#include "duktape-jsi.h"
#include "jsi/jsi.h"
#include <algorithm>
#include <cassert>

using namespace facebook;

DuktapeRuntime::HostFunctionMapType *DuktapeRuntime::hostFunctions =
    new HostFunctionMapType;
DuktapeRuntime::HostObjectMapType *DuktapeRuntime::hostObjects =
    new HostObjectMapType;

DuktapeRuntime::DuktapeRuntime() { ctx = duk_create_heap_default(); }
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

facebook::jsi::Value DuktapeRuntime::evaluateJavaScript(
    const std::shared_ptr<const facebook::jsi::Buffer> &buffer,
    const std::string &sourceUrl) {
  auto err =
      duk_peval_string(ctx, reinterpret_cast<const char *>(buffer->data()));

  if (err) {
    assert(duk_is_error(ctx, -1));
    duk_safe_to_string(ctx, -1);
    std::string message(duk_get_string(ctx, -1));
    std::string stack("[STACK]");
    throw facebook::jsi::JSError(*this, message, stack);
  } else {
    return topOfStackToValue();
  }
}

std::string DuktapeRuntime::description() { return "Duktape 2.7 Runtime"; }

bool DuktapeRuntime::isInspectable() { return false; }

facebook::jsi::Object DuktapeRuntime::global() {
  duk_push_global_object(ctx);
  return wrap<DuktapeObject>();
}

facebook::jsi::PropNameID
DuktapeRuntime::createPropNameIDFromAscii(const char *str, size_t length) {
  std::string newStr(str, length);
  duk_push_string(ctx, newStr.c_str());
  return wrap<DuktapePropNameID>();
}

facebook::jsi::PropNameID
DuktapeRuntime::createPropNameIDFromUtf8(const uint8_t *utf8, size_t length) {
  std::string utf8Str((char *)utf8, length);
  dukPushUtf8String(std::string((const char *)utf8, length));
  return wrap<DuktapePropNameID>();
}

facebook::jsi::PropNameID
DuktapeRuntime::createPropNameIDFromString(const facebook::jsi::String &str) {
  duk_push_heapptr(ctx, ptr(str));
  return wrap<DuktapePropNameID>();
}

facebook::jsi::Runtime::PointerValue *
DuktapeRuntime::cloneSymbol(const facebook::jsi::Runtime::PointerValue *pv) {
  return DuktapePointerValue::clone(pv);
}

facebook::jsi::Runtime::PointerValue *
DuktapeRuntime::cloneString(const facebook::jsi::Runtime::PointerValue *pv) {
  return DuktapePointerValue::clone(pv);
}

facebook::jsi::Runtime::PointerValue *
DuktapeRuntime::cloneObject(const facebook::jsi::Runtime::PointerValue *pv) {
  return DuktapePointerValue::clone(pv);
}

facebook::jsi::Runtime::PointerValue *DuktapeRuntime::clonePropNameID(
    const facebook::jsi::Runtime::PointerValue *pv) {
  return DuktapePointerValue::clone(pv);
}

facebook::jsi::String DuktapeRuntime::createStringFromAscii(const char *str,
                                                            size_t length) {
  std::string newStr(str, length);
  duk_push_string(ctx, newStr.c_str());
  return wrap<DuktapeString>();
}

facebook::jsi::String DuktapeRuntime::createStringFromUtf8(const uint8_t *utf8,
                                                           size_t length) {
  std::string utf8Str((char *)utf8, length);
  dukPushUtf8String(utf8Str);
  return wrap<DuktapeString>();
}

facebook::jsi::Object DuktapeRuntime::createObject() {
  duk_push_object(ctx);
  return wrap<DuktapeObject>();
}

facebook::jsi::Object
DuktapeRuntime::createObject(std::shared_ptr<facebook::jsi::HostObject> ho) {
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
  return wrap<DuktapeObject>();
}

std::shared_ptr<facebook::jsi::HostObject>
DuktapeRuntime::getHostObject(const facebook::jsi::Object &obj) {
  assert(obj.isHostObject(*this));
  return hostObjects->at(ptr(obj))->ho;
}

std::string DuktapeRuntime::utf8(const facebook::jsi::String &str) {
  return dukCopyStringAsUtf8(idx(str));
}

std::string DuktapeRuntime::utf8(const facebook::jsi::PropNameID &prop) {
  return dukCopyStringAsUtf8(idx(prop));
}

bool DuktapeRuntime::compare(const facebook::jsi::PropNameID &a,
                             const facebook::jsi::PropNameID &b) {
  return a.utf8(*this).compare(b.utf8(*this)) == 0;
}

facebook::jsi::HostFunctionType &
DuktapeRuntime::getHostFunction(const facebook::jsi::Function &func) {
  assert(func.isHostFunction(*this));
  return hostFunctions->at(ptr(func))->func;
}

facebook::jsi::Value
DuktapeRuntime::getProperty(const facebook::jsi::Object &obj,
                            const facebook::jsi::PropNameID &name) {
  auto objIndex = idx(obj);
  duk_get_prop_string(ctx, objIndex, name.utf8(*this).c_str());
  return topOfStackToValue();
}

facebook::jsi::Value
DuktapeRuntime::getProperty(const facebook::jsi::Object &obj,
                            const facebook::jsi::String &name) {
  auto objIndex = idx(obj);
  duk_get_prop_string(ctx, objIndex, name.utf8(*this).c_str());
  return topOfStackToValue();
}

bool DuktapeRuntime::hasProperty(const facebook::jsi::Object &obj,
                                 const facebook::jsi::PropNameID &name) {
  return duk_has_prop_heapptr(ctx, idx(obj), ptr(name));
}

bool DuktapeRuntime::hasProperty(const facebook::jsi::Object &obj,
                                 const facebook::jsi::String &name) {
  return duk_has_prop_heapptr(ctx, idx(obj), ptr(name));
}

void DuktapeRuntime::setPropertyValue(facebook::jsi::Object &obj,
                                      const facebook::jsi::PropNameID &name,
                                      const facebook::jsi::Value &value) {
  size_t objIndex = idx(obj);
  assert(duk_is_object(ctx, objIndex));
  duk_dup(ctx, idx(name));
  dukPushJsiValue(value);
  duk_put_prop(ctx, objIndex);
}

void DuktapeRuntime::setPropertyValue(facebook::jsi::Object &obj,
                                      const facebook::jsi::String &key,
                                      const facebook::jsi::Value &value) {
  int objIndex = idx(obj);
  assert(duk_is_object(ctx, objIndex));
  duk_dup(ctx, idx(key));
  dukPushJsiValue(value);
  duk_put_prop(ctx, objIndex);
}

bool DuktapeRuntime::isArray(const facebook::jsi::Object &obj) const {
  return duk_is_array(ctx, idx(obj));
}

bool DuktapeRuntime::isFunction(const facebook::jsi::Object &obj) const {
  return duk_is_function(ctx, idx(obj));
}

bool DuktapeRuntime::isHostFunction(const facebook::jsi::Function &func) const {
  return hostFunctions->find(ptr(func)) != hostFunctions->end();
}

facebook::jsi::Array
DuktapeRuntime::getPropertyNames(const facebook::jsi::Object &obj) {
  duk_push_array(ctx);
  auto arrIndex = duk_normalize_index(ctx, -1);
  auto objIndex = idx(obj);
  duk_enum(ctx, objIndex, 0);
  auto enum_index = duk_normalize_index(ctx, -1);
  for (unsigned int i = 0; duk_next(ctx, enum_index, 0); i++) {
    duk_put_prop_index(ctx, arrIndex, i);
  }
  duk_pop(ctx); // pop the enum
  return topOfStackToValue().getObject(*this).getArray(*this);
}

facebook::jsi::Array DuktapeRuntime::createArray(size_t length) {
  duk_push_array(ctx);
  auto arrIndex = duk_normalize_index(ctx, -1);
  for (size_t i = 0; i < length; i++) {
    duk_push_undefined(ctx);
    duk_put_prop_index(ctx, arrIndex, i);
  }
  return topOfStackToValue().getObject(*this).getArray(*this);
}

size_t DuktapeRuntime::size(const facebook::jsi::Array &arr) {
  return duk_get_length(ctx, idx(arr));
}

facebook::jsi::Value
DuktapeRuntime::getValueAtIndex(const facebook::jsi::Array &ary, size_t i) {
  auto arrIndex = idx(ary);
  duk_get_prop_index(ctx, arrIndex, i);
  return this->topOfStackToValue();
}

void DuktapeRuntime::setValueAtIndexImpl(facebook::jsi::Array &ary, size_t i,
                                         const facebook::jsi::Value &value) {
  auto objIndex = idx(ary);
  assert(duk_is_object(ctx, objIndex));
  assert(duk_is_array(ctx, objIndex));
  dukPushJsiValue(value);
  duk_put_prop_index(ctx, objIndex, i);
}

facebook::jsi::Function DuktapeRuntime::createFunctionFromHostFunction(
    const facebook::jsi::PropNameID &name, unsigned int paramCount,
    facebook::jsi::HostFunctionType func) {
  auto hf = std::make_shared<DuktapeHostFunction>(this, func);
  duk_push_c_function(ctx, DuktapeRuntime::hostFunctionProxy, DUK_VARARGS);
  assert(duk_is_object(ctx, -1));
  assert(duk_is_function(ctx, -1));
  auto funcPointer = duk_get_heapptr(ctx, -1);
  hostFunctions->emplace(funcPointer, hf);
  return wrap<DuktapeObject>().asFunction(*this);
}

facebook::jsi::Value DuktapeRuntime::call(const facebook::jsi::Function &func,
                                          const facebook::jsi::Value &jsThis,
                                          const facebook::jsi::Value *args,
                                          size_t count) {
  dukPushJsiPtrValue(ctx, std::move(func));
  dukPushJsiValue(std::move(jsThis));
  for (unsigned int i = 0; i < count; ++i) {
    dukPushJsiValue(args[i]);
  }
  auto err = duk_pcall_method(ctx, count);
  if (err) {
    throw facebook::jsi::JSError(*this, duk_safe_to_string(ctx, -1));
  }
  return topOfStackToValue();
}

facebook::jsi::Value DuktapeRuntime::topOfStackToValue() {
  return stackToValue(-1);
}

facebook::jsi::Value DuktapeRuntime::stackToValue(int stack_index) {
  if (duk_is_number(ctx, stack_index)) {
    return facebook::jsi::Value(duk_get_number(ctx, stack_index));
  } else if (duk_is_boolean(ctx, stack_index)) {
    return facebook::jsi::Value((bool)duk_get_boolean(ctx, stack_index));
  } else if (duk_is_symbol(ctx, stack_index)) {
    return facebook::jsi::Value(wrap<DuktapeSymbol>(stack_index));
  } else if (duk_is_string(ctx, stack_index)) {
    return facebook::jsi::Value(wrap<DuktapeString>(stack_index));
  } else if (duk_is_null(ctx, stack_index)) {
    return facebook::jsi::Value(nullptr);
  } else if (duk_is_object(ctx, stack_index)) {
    return facebook::jsi::Value(wrap<DuktapeObject>(stack_index));
  } else if (duk_is_undefined(ctx, stack_index)) {
    return facebook::jsi::Value();
  }
  throw std::logic_error("unknown duktype");
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
  std::vector<facebook::jsi::Value> args;

  auto proxyPointer = duk_get_heapptr(ctx, -1);
  assert(hostObjects->find(proxyPointer) != hostObjects->end());
  auto hostObj = hostObjects->at(proxyPointer);
  assert(hostObj != nullptr);
  auto dt = hostObj->rt;
  assert(dt != nullptr);
  auto ho = hostObj->ho;
  assert(ho != nullptr);

  for (int i = 0; i < n; ++i) {
    size_t stackIndex = duk_normalize_index(ctx, i - n);
    args.push_back(dt->stackToValue(stackIndex));
  }

  if (trap == "ownKeys") {
    auto names = ho->getPropertyNames(*dt);

    // uniquify the names (omg this is terrible)
    std::vector<std::string> namesUtf8;
    std::transform(
        names.begin(), names.end(), std::back_inserter(namesUtf8),
        [&](facebook::jsi::PropNameID &prop) { return prop.utf8(*dt); });

    std::sort(namesUtf8.begin(), namesUtf8.end());
    namesUtf8.erase(std::unique(namesUtf8.begin(), namesUtf8.end()),
                    namesUtf8.end());

    auto result = facebook::jsi::Array(*dt, namesUtf8.size());
    for (size_t i = 0; i < namesUtf8.size(); i++) {
      auto str = dt->createStringFromUtf8((uint8_t *)namesUtf8[i].c_str(),
                                          namesUtf8[i].size());
      result.setValueAtIndex(*dt, i, str);
    }

    dt->dukPushJsiValue(std::move(result));
  } else {
    if (!args[1].isString()) {
      auto numString = std::to_string((int)args[1].getNumber());
      args[1] = dt->createStringFromAscii(numString.c_str(),
                                          strlen(numString.c_str()));
    }

    auto propName =
        facebook::jsi::PropNameID::forString(*dt, args[1].getString(*dt));

    if (trap == "get") {
      try {
        auto result = ho->get(*dt, propName);
        dt->dukPushJsiValue(result);
      } catch (std::exception &e) {
        throw facebook::jsi::JSError(*dt, e.what(), "");
      }
    } else if (trap == "set") {
      try {
        ho->set(*dt, propName, args[2]);
        dt->dukPushJsiValue(args[2]);
      } catch (std::exception &e) {
        throw facebook::jsi::JSError(*dt, e.what(), "");
      }
    } else {
      throw std::runtime_error("unknown trap");
    }
  }
  return 1;
}

duk_ret_t DuktapeRuntime::hostFunctionProxy(duk_context *ctx) {
  int n = duk_get_top(ctx);
  std::vector<facebook::jsi::Value> args;

  duk_push_current_function(ctx);
  auto hostFunc = hostFunctions->at(duk_get_heapptr(ctx, -1));
  duk_pop(ctx);
  auto dt = hostFunc->rt;
  auto func = hostFunc->func;

  for (int i = 0; i < n; ++i) {
    size_t stackIndex = duk_normalize_index(ctx, i - n);
    args.push_back(dt->stackToValue(stackIndex));
  }

  auto result = func(*dt, facebook::jsi::Value(), args.data(), n);
  dt->dukPushJsiValue(result);
  return 1;
}
