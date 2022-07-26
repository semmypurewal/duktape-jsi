#include "duktape-jsi.h"
#include "cesu8/cesu8.h"
#include <algorithm>
#include <cassert>

namespace DuktapeJSI {

DuktapeRuntime::HostFunctionMapType *DuktapeRuntime::hostFunctions =
    new HostFunctionMapType;
DuktapeRuntime::HostObjectMapType *DuktapeRuntime::hostObjects =
    new HostObjectMapType;

DuktapeRuntime::DuktapeRuntime() {
  ctx = duk_create_heap_default();
  refCounts_ = new std::map<void *, size_t>();
  scopeStack_ = new std::stack<std::shared_ptr<DuktapeScopeState>>();
  pushDuktapeScope();
}

DuktapeRuntime::~DuktapeRuntime() {
  // remove host functions from global map
  std::vector<void *> toRemove;
  for (auto &pair : *hostFunctions) {
    if (pair.second->rt == this) {
      toRemove.push_back(pair.first);
    }
  }

  for (auto index : toRemove) {
    hostFunctions->erase(index);
  }

  // remove host objects from global map
  toRemove.clear();
  for (auto &pair : *hostObjects) {
    if (pair.second->rt == this) {
      toRemove.push_back(pair.first);
    }
  }

  for (auto index : toRemove) {
    hostObjects->erase(index);
  }

  // destroy duktape heap
  duk_destroy_heap(ctx);
}

jsi::Value DuktapeRuntime::evaluateJavaScript(
    const std::shared_ptr<const jsi::Buffer> &buffer,
    const std::string &sourceUrl) {

  duk_push_string(ctx, reinterpret_cast<const char *>(buffer->data()));
  duk_push_string(ctx, sourceUrl.c_str());
  auto err = duk_pcompile(ctx, 0);
  if (err) {
    throwValueOnTopOfStack();
  }
  err = duk_pcall(ctx, 0);

  if (err) {
    throwValueOnTopOfStack();
  }
  return topOfStackToValue();
}

std::shared_ptr<const jsi::PreparedJavaScript>
DuktapeRuntime::prepareJavaScript(
    const std::shared_ptr<const jsi::Buffer> &buffer, std::string sourceURL) {
  return std::make_shared<jsi::SourceJavaScriptPreparation>(buffer, sourceURL);
}

jsi::Value DuktapeRuntime::evaluatePreparedJavaScript(
    const std::shared_ptr<const jsi::PreparedJavaScript> &js) {
  auto sjsp =
      std::dynamic_pointer_cast<const jsi::SourceJavaScriptPreparation>(js);
  return evaluateJavaScript(
      std::make_shared<jsi::StringBuffer>((const char *)sjsp->data()),
      sjsp->sourceURL());
}

std::string DuktapeRuntime::description() { return "Duktape 2.7 Runtime"; }

bool DuktapeRuntime::isInspectable() { return false; }

jsi::Object DuktapeRuntime::global() {
  duk_push_global_object(ctx);
  return wrap<jsi::Object>();
}

jsi::PropNameID DuktapeRuntime::createPropNameIDFromAscii(const char *str,
                                                          size_t length) {
  std::string newStr(str, length);
  duk_push_string(ctx, newStr.c_str());
  return wrap<jsi::PropNameID>();
}

jsi::PropNameID DuktapeRuntime::createPropNameIDFromUtf8(const uint8_t *utf8,
                                                         size_t length) {
  std::string utf8Str((char *)utf8, length);
  dukPushUtf8String(std::string((const char *)utf8, length));
  return wrap<jsi::PropNameID>();
}

jsi::PropNameID
DuktapeRuntime::createPropNameIDFromString(const jsi::String &str) {
  duk_push_heapptr(ctx, ptr(str));
  return wrap<jsi::PropNameID>();
}

jsi::Runtime::PointerValue *
DuktapeRuntime::cloneSymbol(const jsi::Runtime::PointerValue *pv) {
  return DuktapePointerValue::clone(pv);
}

jsi::Runtime::PointerValue *
DuktapeRuntime::cloneString(const jsi::Runtime::PointerValue *pv) {
  return DuktapePointerValue::clone(pv);
}

jsi::Runtime::PointerValue *
DuktapeRuntime::cloneObject(const jsi::Runtime::PointerValue *pv) {
  return DuktapePointerValue::clone(pv);
}

jsi::Runtime::PointerValue *
DuktapeRuntime::clonePropNameID(const jsi::Runtime::PointerValue *pv) {
  return DuktapePointerValue::clone(pv);
}

std::string DuktapeRuntime::symbolToString(const jsi::Symbol &s) {
  assert(duk_is_symbol(ctx, idx(s)));
  auto dukSym = dukCopyStringAsUtf8(idx(s));
  auto dukSymCStr = (uint8_t *)dukSym.c_str();
  assert(dukSymCStr[0] == 0x81);
  auto end = 1;
  while (dukSymCStr[end] != 0xff) {
    end++;
  }
  return "Symbol(" + dukSym.substr(1, end - 1) + ")";
}

jsi::String DuktapeRuntime::createStringFromAscii(const char *str,
                                                  size_t length) {
  std::string newStr(str, length);
  duk_push_string(ctx, newStr.c_str());
  return wrap<jsi::String>();
}

jsi::String DuktapeRuntime::createStringFromUtf8(const uint8_t *utf8,
                                                 size_t length) {
  std::string utf8Str((char *)utf8, length);
  dukPushUtf8String(utf8Str);
  return wrap<jsi::String>();
}

jsi::Object DuktapeRuntime::createObject() {
  duk_push_object(ctx);
  return wrap<jsi::Object>();
}

jsi::Object DuktapeRuntime::createObject(std::shared_ptr<jsi::HostObject> ho) {
  duk_push_object(ctx); // target
  auto targetHeapPtr = duk_get_heapptr(ctx, -1);
  duk_push_object(ctx); // handler
  duk_push_c_function(ctx, hostObjectGetProxy, 3);
  duk_put_prop_string(ctx, -2, "get");
  duk_push_c_function(ctx, hostObjectSetProxy, 4);
  duk_put_prop_string(ctx, -2, "set");
  duk_push_c_function(ctx, hostObjectOwnKeysProxy, 1);
  duk_put_prop_string(ctx, -2, "ownKeys");

  duk_push_proxy(ctx, 0);
  auto proxyHeapPtr = duk_get_heapptr(ctx, -1);

  auto dho = std::make_shared<DuktapeHostObject>(this, ho);
  // we add the HostObject to the map twice so we can look it up by both the
  // proxy and the target
  hostObjects->emplace(proxyHeapPtr, dho);
  hostObjects->emplace(targetHeapPtr, dho);
  return wrap<jsi::Object>();
}

std::shared_ptr<jsi::HostObject>
DuktapeRuntime::getHostObject(const jsi::Object &obj) {
  assert(obj.isHostObject(*this));
  return hostObjects->at(ptr(obj))->ho;
}

std::string DuktapeRuntime::utf8(const jsi::String &str) {
  return dukCopyStringAsUtf8(idx(str));
}

std::string DuktapeRuntime::utf8(const jsi::PropNameID &prop) {
  return dukCopyStringAsUtf8(idx(prop));
}

bool DuktapeRuntime::compare(const jsi::PropNameID &a,
                             const jsi::PropNameID &b) {
  return a.utf8(*this).compare(b.utf8(*this)) == 0;
}

jsi::HostFunctionType &
DuktapeRuntime::getHostFunction(const jsi::Function &func) {
  assert(func.isHostFunction(*this));
  return hostFunctions->at(ptr(func))->func;
}

jsi::Value DuktapeRuntime::getProperty(const jsi::Object &obj,
                                       const jsi::PropNameID &name) {
  auto objIndex = idx(obj);
  duk_get_prop_string(ctx, objIndex, name.utf8(*this).c_str());
  return topOfStackToValue();
}

jsi::Value DuktapeRuntime::getProperty(const jsi::Object &obj,
                                       const jsi::String &name) {
  auto objIndex = idx(obj);
  duk_get_prop_string(ctx, objIndex, name.utf8(*this).c_str());
  return topOfStackToValue();
}

bool DuktapeRuntime::hasProperty(const jsi::Object &obj,
                                 const jsi::PropNameID &name) {
  return duk_has_prop_heapptr(ctx, idx(obj), ptr(name));
}

bool DuktapeRuntime::hasProperty(const jsi::Object &obj,
                                 const jsi::String &name) {
  return duk_has_prop_heapptr(ctx, idx(obj), ptr(name));
}

void DuktapeRuntime::setPropertyValue(jsi::Object &obj,
                                      const jsi::PropNameID &name,
                                      const jsi::Value &value) {
  size_t objIndex = idx(obj);
  assert(duk_is_object(ctx, objIndex));
  duk_dup(ctx, idx(name));
  dukPushJsiValue(value);
  duk_put_prop(ctx, objIndex);
}

void DuktapeRuntime::setPropertyValue(jsi::Object &obj, const jsi::String &key,
                                      const jsi::Value &value) {
  int objIndex = idx(obj);
  assert(duk_is_object(ctx, objIndex));
  duk_dup(ctx, idx(key));
  dukPushJsiValue(value);
  duk_put_prop(ctx, objIndex);
}

bool DuktapeRuntime::isArray(const jsi::Object &obj) const {
  return duk_is_array(ctx, idx(obj));
}

bool DuktapeRuntime::isFunction(const jsi::Object &obj) const {
  return duk_is_function(ctx, idx(obj));
}

bool DuktapeRuntime::isHostObject(const jsi::Object &obj) const {
  return hostObjects->find(ptr(obj)) != hostObjects->end();
}

bool DuktapeRuntime::isHostFunction(const jsi::Function &func) const {
  return hostFunctions->find(ptr(func)) != hostFunctions->end();
}

jsi::Array DuktapeRuntime::getPropertyNames(const jsi::Object &obj) {
  auto objIndex = idx(obj);
  duk_push_array(ctx);
  auto arrIndex = duk_normalize_index(ctx, -1);
  duk_enum(ctx, objIndex, 0);
  auto enum_index = duk_normalize_index(ctx, -1);
  for (unsigned int i = 0; duk_next(ctx, enum_index, 0); i++) {
    duk_put_prop_index(ctx, arrIndex, i);
  }
  duk_pop(ctx); // pop the enum
  return topOfStackToValue().getObject(*this).getArray(*this);
}

jsi::Array DuktapeRuntime::createArray(size_t length) {
  duk_push_array(ctx);
  auto arrIndex = duk_normalize_index(ctx, -1);
  for (size_t i = 0; i < length; i++) {
    duk_push_undefined(ctx);
    duk_put_prop_index(ctx, arrIndex, i);
  }
  return topOfStackToValue().getObject(*this).getArray(*this);
}

size_t DuktapeRuntime::size(const jsi::Array &arr) {
  return duk_get_length(ctx, idx(arr));
}

jsi::Value DuktapeRuntime::getValueAtIndex(const jsi::Array &ary, size_t i) {
  auto arrIndex = idx(ary);
  duk_get_prop_index(ctx, arrIndex, i);
  return this->topOfStackToValue();
}

void DuktapeRuntime::setValueAtIndexImpl(jsi::Array &ary, size_t i,
                                         const jsi::Value &value) {
  auto objIndex = idx(ary);
  assert(duk_is_object(ctx, objIndex));
  assert(duk_is_array(ctx, objIndex));
  dukPushJsiValue(value);
  duk_put_prop_index(ctx, objIndex, i);
}

jsi::Function
DuktapeRuntime::createFunctionFromHostFunction(const jsi::PropNameID &name,
                                               unsigned int paramCount,
                                               jsi::HostFunctionType func) {
  auto hf = std::make_shared<DuktapeHostFunction>(this, func);
  duk_push_c_function(ctx, DuktapeRuntime::hostFunctionProxy, DUK_VARARGS);
  assert(duk_is_object(ctx, -1));
  assert(duk_is_function(ctx, -1));
  auto hostFunctionIndex = duk_normalize_index(ctx, -1);

  duk_push_string(ctx, "name");
  duk_dup(ctx, idx(name));
  duk_def_prop(ctx, hostFunctionIndex, DUK_DEFPROP_HAVE_VALUE);

  duk_push_string(ctx, "length");
  duk_push_number(ctx, paramCount);
  duk_def_prop(ctx, hostFunctionIndex, DUK_DEFPROP_HAVE_VALUE);

  auto funcPointer = duk_get_heapptr(ctx, hostFunctionIndex);
  hostFunctions->emplace(funcPointer, hf);
  return wrap<jsi::Object>().asFunction(*this);
}

jsi::Value DuktapeRuntime::call(const jsi::Function &func,
                                const jsi::Value &jsThis,
                                const jsi::Value *args, size_t count) {
  dukPushJsiPtrValue(std::move(func));
  dukPushJsiValue(std::move(jsThis));
  for (unsigned int i = 0; i < count; ++i) {
    dukPushJsiValue(args[i]);
  }
  auto err = duk_pcall_method(ctx, count);
  if (err) {
    throwValueOnTopOfStack();
  }
  return topOfStackToValue();
}

jsi::Value DuktapeRuntime::callAsConstructor(const jsi::Function &constructor,
                                             const jsi::Value *args,
                                             size_t count) {
  duk_dup(ctx, idx(constructor));
  for (size_t i = 0; i < count; i++) {
    dukPushJsiValue(args[i]);
  }
  duk_new(ctx, count);
  return topOfStackToValue();
}

bool DuktapeRuntime::strictEquals(const jsi::Symbol &a,
                                  const jsi::Symbol &b) const {
  return duk_strict_equals(ctx, idx(a), idx(b));
}

bool DuktapeRuntime::strictEquals(const jsi::String &a,
                                  const jsi::String &b) const {
  return duk_strict_equals(ctx, idx(a), idx(b));
}

bool DuktapeRuntime::strictEquals(const jsi::Object &a,
                                  const jsi::Object &b) const {
  return duk_strict_equals(ctx, idx(a), idx(b));
}

bool DuktapeRuntime::instanceOf(const jsi::Object &o, const jsi::Function &f) {
  return duk_instanceof(ctx, idx(o), idx(f));
}

size_t DuktapeRuntime::getStackTop() { return duk_get_top(ctx); }

std::string DuktapeRuntime::stackEltToString(int idx) {
  std::string result = std::to_string(idx);

  if ((duk_is_object(ctx, idx) || duk_is_string(ctx, idx)) &&
      refCounts_->find(duk_get_heapptr(ctx, idx)) != refCounts_->end()) {
    auto refCount = refCounts_->at(duk_get_heapptr(ctx, idx));
    result += "(" + std::to_string(refCount) + ")";
  } else {
    result += "(*)";
  }

  result += ":\t";
  if (duk_is_boolean(ctx, idx)) {
    if (duk_get_boolean(ctx, idx)) {
      result += "true";
    } else {
      result += "false";
    }
  } else if (duk_is_number(ctx, idx)) {
    result += std::to_string(duk_get_number(ctx, idx));
  } else if (duk_is_undefined(ctx, idx)) {
    result += "undefined";
  } else if (duk_is_null(ctx, idx)) {
    result += "null";
  } else if (duk_is_string(ctx, idx)) {
    result += std::string("'") + duk_get_string(ctx, idx) + std::string("'");
  } else if (duk_is_function(ctx, idx)) {
    result += "[Object/Function]";
  } else if (duk_is_array(ctx, idx)) {
    result += "[Object/Array]";
  } else if (duk_is_object(ctx, idx)) {
    result += "[Object/Object]";
  } else {
    result += "[unknown type]";
  }

  return result;
}

std::string DuktapeRuntime::stackToString() {
  std::string result = "*************************\n";
  size_t top = duk_normalize_index(ctx, -1);
  for (int i = top; i >= 0; i--) {
    result += stackEltToString(i) + "\n";
  }
  result += "*************************";
  return result;
}

jsi::Value DuktapeRuntime::topOfStackToValue() { return stackToValue(-1); }

jsi::Value DuktapeRuntime::stackToValue(int stackIndex) {
  if (duk_is_number(ctx, stackIndex)) {
    return jsi::Value(duk_get_number(ctx, stackIndex));
  } else if (duk_is_boolean(ctx, stackIndex)) {
    return jsi::Value((bool)duk_get_boolean(ctx, stackIndex));
  } else if (duk_is_symbol(ctx, stackIndex)) {
    return jsi::Value(wrap<jsi::Symbol>(stackIndex));
  } else if (duk_is_string(ctx, stackIndex)) {
    return jsi::Value(wrap<jsi::String>(stackIndex));
  } else if (duk_is_null(ctx, stackIndex)) {
    return jsi::Value(nullptr);
  } else if (duk_is_object(ctx, stackIndex)) {
    return jsi::Value(wrap<jsi::Object>(stackIndex));
  } else if (duk_is_undefined(ctx, stackIndex)) {
    return jsi::Value();
  }
  throw std::logic_error("unknown duktype");
}

template <typename T> T DuktapeRuntime::wrap(int stackIndex) {
  auto idx = duk_normalize_index(ctx, stackIndex);
  return DuktapeWrapper<T>(*this, idx);
}

size_t DuktapeRuntime::idx(const jsi::Pointer &p) const {
  return static_cast<const DuktapeWrapper<const jsi::Pointer> &>(p).idx();
}

void *DuktapeRuntime::ptr(const jsi::Pointer &p) const {
  return static_cast<const DuktapeWrapper<const jsi::Pointer> &>(p).ptr();
}

void DuktapeRuntime::increaseRefCount(void *ref) {
  if (refCounts_->find(ref) == refCounts_->end()) {
    refCounts_->emplace(ref, 0);
  }
  refCounts_->at(ref)++;
}

void DuktapeRuntime::decreaseRefCount(void *ref) {
  refCounts_->at(ref)--;

  if (refCounts_->at(ref) == 0) {
    refCounts_->erase(ref);
  }
}

bool DuktapeRuntime::hasReference(void *dukPtr) {
  if (dukPtr && refCounts_->find(dukPtr) != refCounts_->end()) {
    return true;
  }

  return false;
}

void DuktapeRuntime::dukPushJsiValue(const jsi::Value &value) {
  if (value.isUndefined()) {
    duk_push_undefined(ctx);
  } else if (value.isNull()) {
    duk_push_null(ctx);
  } else if (value.isBool()) {
    duk_push_boolean(ctx, value.getBool());
  } else if (value.isNumber()) {
    duk_push_number(ctx, value.getNumber());
  } else if (value.isString()) {
    dukPushJsiPtrValue(value.getString(*this));
  } else if (value.isSymbol()) {
    dukPushJsiPtrValue(value.getSymbol(*this));
  } else if (value.isObject()) {
    dukPushJsiPtrValue(value.getObject(*this));
  } else {
    throw std::logic_error("unknown duk type");
  }
}

template <typename T> void DuktapeRuntime::dukPushJsiPtrValue(T &&value) {
  duk_push_heapptr(ctx, ptr(value));
}

void DuktapeRuntime::dukPushUtf8String(const std::string &utf8) {
  auto utf8_cstr = utf8.c_str();
  auto len = cesu8_len_from_utf8(utf8_cstr, strlen(utf8_cstr)) + 1;
  auto cesu8 = (char *)malloc(len);
  copy_utf8_as_cesu8(cesu8, utf8_cstr);
  duk_push_string(ctx, (const char *)cesu8);
  free(cesu8);
}

std::string DuktapeRuntime::dukCopyStringAsUtf8(int stackIndex) {
  auto cesu8 = duk_get_string(ctx, stackIndex);
  auto len = utf8_len_from_cesu8(cesu8, strlen(cesu8)) + 1;
  auto utf8 = (char *)malloc(len);
  copy_cesu8_as_utf8(utf8, cesu8);
  auto result = std::string((const char *)utf8);
  free(utf8);
  return result;
}

void DuktapeRuntime::popDuktapeScope() {
  auto poppedScope = scopeStack_->top();
  scopeStack_->pop();
  poppedScope->invalidate();
}

std::shared_ptr<DuktapeRuntime::DuktapeScopeState>
DuktapeRuntime::pushDuktapeScope() {
  auto result = std::make_shared<DuktapeScopeState>(*this);
  scopeStack_->push(result);
  return result;
}

duk_ret_t DuktapeRuntime::hostObjectGetProxy(duk_context *ctx) {
  return hostObjectProxy(std::string("get"), ctx);
}

duk_ret_t DuktapeRuntime::hostObjectSetProxy(duk_context *ctx) {
  return hostObjectProxy(std::string("set"), ctx);
}

duk_ret_t DuktapeRuntime::hostObjectOwnKeysProxy(duk_context *ctx) {
  return hostObjectProxy(std::string("ownKeys"), ctx);
}

duk_ret_t DuktapeRuntime::hostObjectProxy(std::string trap, duk_context *ctx) {
  int n = duk_get_top(ctx);
  std::vector<jsi::Value> args;

  auto proxyPointer = duk_get_heapptr(ctx, -1);
  assert(hostObjects->find(proxyPointer) != hostObjects->end());
  auto hostObj = hostObjects->at(proxyPointer);
  assert(hostObj != nullptr);
  auto dt = hostObj->rt;
  assert(dt != nullptr);
  auto ho = hostObj->ho;
  assert(ho != nullptr);

  auto scope = dt->pushDuktapeScope();

  for (int i = 0; i < n; ++i) {
    size_t stackIndex = duk_normalize_index(ctx, i - n);
    args.push_back(dt->stackToValue(stackIndex));
  }

  if (trap == "ownKeys") {
    auto names = ho->getPropertyNames(*dt);

    // uniquify the names (omg this is terrible)
    std::vector<std::string> namesUtf8;
    std::transform(names.begin(), names.end(), std::back_inserter(namesUtf8),
                   [&](jsi::PropNameID &prop) { return prop.utf8(*dt); });

    std::sort(namesUtf8.begin(), namesUtf8.end());
    namesUtf8.erase(std::unique(namesUtf8.begin(), namesUtf8.end()),
                    namesUtf8.end());

    auto result = jsi::Array(*dt, namesUtf8.size());
    for (size_t i = 0; i < namesUtf8.size(); i++) {
      auto str = dt->createStringFromUtf8((uint8_t *)namesUtf8[i].c_str(),
                                          namesUtf8[i].size());
      result.setValueAtIndex(*dt, i, str);
    }

    scope->pushReturnValue(std::move(result));
  } else {
    if (!args[1].isString()) {
      auto numString = std::to_string((int)args[1].getNumber());
      args[1] = dt->createStringFromAscii(numString.c_str(),
                                          strlen(numString.c_str()));
    }

    auto propName = jsi::PropNameID::forString(*dt, args[1].getString(*dt));

    if (trap == "get") {
      try {
        auto result = ho->get(*dt, propName);
        scope->pushReturnValue(result);
      } catch (std::exception &e) {
        throw jsi::JSError(*dt, e.what(), "");
      }
    } else if (trap == "set") {
      try {
        ho->set(*dt, propName, args[2]);

        if (args[2].isObject() || args[2].isString()) {
          dt->createCppRef(args[2]);
        }

        scope->pushReturnValue(args[2]);
      } catch (std::exception &e) {
        throw jsi::JSError(*dt, e.what(), "");
      }
    } else {
      throw std::runtime_error("unknown trap");
    }
  }
  dt->popDuktapeScope();
  return 1;
}

duk_ret_t DuktapeRuntime::hostFunctionProxy(duk_context *ctx) {
  int n = duk_get_top(ctx);
  std::vector<jsi::Value> args;

  duk_push_current_function(ctx);
  auto hostFunc = hostFunctions->at(duk_get_heapptr(ctx, -1));
  duk_pop(ctx);
  auto dt = hostFunc->rt;
  auto scope = dt->pushDuktapeScope();

  auto func = hostFunc->func;

  for (int i = 0; i < n; ++i) {
    size_t stackIndex = duk_normalize_index(ctx, i - n);
    args.push_back(dt->stackToValue(stackIndex));
  }

  duk_push_this(ctx);
  auto thisValue = dt->stackToValue(duk_normalize_index(ctx, -1));

  try {
    auto result = func(*dt, thisValue, args.data(), args.size());
    scope->pushReturnValue(result);
  } catch (jsi::JSError &e) {
    dt->throwJSError(e.getMessage(), e.getStack());
  } catch (std::exception &e) {
    std::string err(std::string("Exception in HostFunction: ") +
                    std::string(e.what()));
    dt->throwJSError(err);
  }
  dt->popDuktapeScope();
  return 1;
}

// TODO: removeCppRef when the cpp object goes completely out of scope
void DuktapeRuntime::createCppRef(jsi::Value &v) {
  const char *cppRefsKey = "__cppRefs_";

  if (!global().hasProperty(*this, cppRefsKey)) {
    global().setProperty(*this, cppRefsKey, createObject());
  }

  auto cppRefs = global().getProperty(*this, cppRefsKey).asObject(*this);

  void *key = nullptr;
  if (v.isObject()) {
    key = ptr(v.asObject(*this));
  } else if (v.isString()) {
    key = ptr(v.asString(*this));
  }
  cppRefs.setProperty(*this, std::to_string((unsigned long)key).c_str(), v);
}

void throwJSErrorImpl(DuktapeRuntime *dt, duk_context *ctx,
                      std::unique_ptr<std::string> msg,
                      std::unique_ptr<std::string> stack) {
  // If the global Error object is deleted, the JSI tests expect a
  // string to be thrown instead of an Error object.  This doesn't
  // seem necessary for Duktape, since we can still throw Error
  // objects from C, but we'll go with it for now.
  auto error = dt->global().getProperty(*dt, "Error");
  if (error.isObject()) {
    duk_push_error_object(ctx, DUK_ERR_ERROR, msg->c_str());
    auto errIndex = duk_normalize_index(ctx, -1);
    if (stack != nullptr) {
      duk_push_string(ctx, stack->c_str());
      duk_put_prop_string(ctx, errIndex, "stack");
    }
    assert(duk_is_error(ctx, -1));
    duk_throw(ctx);
  } else {
    duk_push_string(ctx, msg->c_str());
    duk_throw(ctx);
  }
}

void DuktapeRuntime::throwJSError(std::string msg) {
  throwJSErrorImpl(this, ctx, std::make_unique<std::string>(msg), nullptr);
}

void DuktapeRuntime::throwJSError(std::string msg, std::string stack) {
  throwJSErrorImpl(this, ctx, std::make_unique<std::string>(msg),
                   std::make_unique<std::string>(stack));
}

void DuktapeRuntime::throwValueOnTopOfStack() {
  std::string dukCallStackLimit("callstack limit");

  auto objIndex = duk_normalize_index(ctx, -1);
  if (duk_is_error(ctx, objIndex)) {
    duk_get_prop_string(ctx, objIndex, "stack");
    std::string stack(duk_get_string(ctx, -1));

    duk_get_prop_string(ctx, objIndex, "message");
    duk_safe_to_string(ctx, -1);
    std::string message(duk_get_string(ctx, -1));

    // The JSI tests expect the word 'exceeded' to appear in the error
    // message associated with a stack overflow. This replacement gets
    // that test passing, but it's probably not necessary.
    auto index = message.find(dukCallStackLimit);
    if (index != std::string::npos) {
      message.replace(index, dukCallStackLimit.size(),
                      "Maximum call stack size exceeded");
    }
    throw jsi::JSError(*this, message, stack);
  } else {
    throw jsi::JSError(*this, duk_safe_to_string(ctx, -1));
  }
}
} // namespace DuktapeJSI
