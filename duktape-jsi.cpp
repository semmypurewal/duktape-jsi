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
    duk_safe_to_string(ctx, -1);
    std::string message(duk_get_string(ctx, -1));
    std::string stack("");
    throw facebook::jsi::JSError(*this, message, stack);
  } else {
    return topOfStackToValue();
  }
}

std::string DuktapeRuntime::description() { return "Duktape 2.7 Runtime"; }

bool DuktapeRuntime::isInspectable() { return false; }

facebook::jsi::Object DuktapeRuntime::global() {
  duk_push_global_object(ctx);
  auto result = DuktapeObject(duk_get_heapptr(ctx, -1));
  duk_pop(ctx);
  return std::move(result);
}

facebook::jsi::PropNameID
DuktapeRuntime::createPropNameIDFromAscii(const char *str, size_t length) {
  std::string newStr(str, length);
  duk_push_string(ctx, newStr.c_str());
  return DuktapePropNameID(duk_get_heapptr(ctx, -1));
}

facebook::jsi::PropNameID
DuktapeRuntime::createPropNameIDFromUtf8(const uint8_t *utf8, size_t length) {
  std::string utf8Str((char *)utf8, length);
  dukPushUtf8String(std::string((const char *)utf8, length));
  return DuktapePropNameID(duk_get_heapptr(ctx, -1));
}

facebook::jsi::PropNameID
DuktapeRuntime::createPropNameIDFromString(const facebook::jsi::String &str) {
  duk_push_heapptr(ctx, DuktapeString::get(str));
  return DuktapePropNameID(duk_get_heapptr(ctx, -1));
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
  return DuktapeString(duk_get_heapptr(ctx, -1));
}

facebook::jsi::String DuktapeRuntime::createStringFromUtf8(const uint8_t *utf8,
                                                           size_t length) {
  std::string utf8Str((char *)utf8, length);
  dukPushUtf8String(utf8Str);
  return DuktapeString(duk_get_heapptr(ctx, -1));
}

facebook::jsi::Object DuktapeRuntime::createObject() {
  duk_push_object(ctx);
  return DuktapeObject(duk_get_heapptr(ctx, -1));
}

facebook::jsi::Object
DuktapeRuntime::createObject(std::shared_ptr<facebook::jsi::HostObject> ho) {
  duk_push_object(ctx); // target
  auto targetHeapPtr = duk_get_heapptr(ctx, -1);
  duk_push_object(ctx); // handler
  duk_push_c_function(ctx, dukHostObjectGetProxyFunction, 3);
  duk_put_prop_string(ctx, -2, "get");
  duk_push_c_function(ctx, dukHostObjectSetProxyFunction, 4);
  duk_put_prop_string(ctx, -2, "set");
  duk_push_c_function(ctx, dukHostObjectOwnKeysProxyFunction, 1);
  duk_put_prop_string(ctx, -2, "ownKeys");

  duk_push_proxy(ctx, 0);
  auto proxyHeapPtr = duk_get_heapptr(ctx, -1);

  auto dho = std::make_shared<DuktapeHostObject>(this, ho);
  // we add the HostObject to the map twice so we can look it up by both the
  // proxy and the target
  hostObjects->emplace(proxyHeapPtr, dho);
  hostObjects->emplace(targetHeapPtr, dho);
  return DuktapeObject(proxyHeapPtr);
}

std::shared_ptr<facebook::jsi::HostObject>
DuktapeRuntime::getHostObject(const facebook::jsi::Object &obj) {
  assert(obj.isHostObject(*this));
  return hostObjects->at(DuktapeObject::get(obj))->ho;
}

std::string DuktapeRuntime::utf8(const facebook::jsi::String &str) {
  duk_push_heapptr(ctx, DuktapeString::get(str));
  auto result = dukCopyStringAsUtf8(-1);
  duk_pop(ctx);
  return result;
}

std::string DuktapeRuntime::utf8(const facebook::jsi::PropNameID &prop) {
  duk_push_heapptr(ctx, DuktapePropNameID::get(prop));
  auto result = dukCopyStringAsUtf8(-1);
  duk_pop(ctx);
  return result;
}

bool DuktapeRuntime::compare(const facebook::jsi::PropNameID &a,
                             const facebook::jsi::PropNameID &b) {
  return a.utf8(*this).compare(b.utf8(*this)) == 0;
}

facebook::jsi::HostFunctionType &
DuktapeRuntime::getHostFunction(const facebook::jsi::Function &func) {
  assert(func.isHostFunction(*this));
  return hostFunctions->at(DuktapeObject::get(func))->func;
}

facebook::jsi::Value
DuktapeRuntime::getProperty(const facebook::jsi::Object &obj,
                            const facebook::jsi::PropNameID &name) {
  duk_push_heapptr(ctx, DuktapeObject::get(obj));
  assert(duk_is_object(ctx, -1));
  auto obj_index = duk_normalize_index(ctx, -1);
  duk_push_heapptr(ctx, DuktapePropNameID::get(name));
  assert(duk_is_object(ctx, obj_index));
  assert(duk_is_string(ctx, -1));
  duk_get_prop(ctx, obj_index);
  auto result = topOfStackToValue();
  duk_pop_2(ctx);
  return result;
}

facebook::jsi::Value
DuktapeRuntime::getProperty(const facebook::jsi::Object &obj,
                            const facebook::jsi::String &name) {
  duk_push_heapptr(ctx, DuktapeObject::get(obj));
  assert(duk_is_object(ctx, -1));
  auto obj_index = duk_normalize_index(ctx, -1);
  duk_push_string(ctx, name.utf8(*this).c_str());
  assert(duk_is_object(ctx, obj_index));
  assert(duk_is_string(ctx, -1));
  duk_get_prop(ctx, obj_index);
  auto result = topOfStackToValue();
  duk_pop_2(ctx);
  return result;
}

bool DuktapeRuntime::hasProperty(const facebook::jsi::Object &obj,
                                 const facebook::jsi::PropNameID &name) {
  duk_push_heapptr(ctx, DuktapeObject::get(obj));
  auto result = duk_has_prop_heapptr(ctx, -1, DuktapePropNameID::get(name));
  duk_pop(ctx);
  return result;
}

bool DuktapeRuntime::hasProperty(const facebook::jsi::Object &obj,
                                 const facebook::jsi::String &name) {
  duk_push_heapptr(ctx, DuktapeObject::get(obj));
  auto result = duk_has_prop_heapptr(ctx, -1, DuktapeString::get(name));
  duk_pop(ctx);
  return result;
}

void DuktapeRuntime::setPropertyValue(facebook::jsi::Object &obj,
                                      const facebook::jsi::PropNameID &name,
                                      const facebook::jsi::Value &value) {
  dukPushJsiPtrValue(ctx, std::move(obj));
  int obj_index = duk_normalize_index(ctx, -1);
  assert(duk_is_object(ctx, obj_index));
  duk_push_heapptr(ctx, DuktapePropNameID::get(name));
  assert(duk_is_string(ctx, -1));
  int key_index = duk_normalize_index(ctx, -1);
  dukPushJsiValue(ctx, value);
  int value_index = duk_normalize_index(ctx, -1);
  duk_pull(ctx, key_index);
  duk_pull(ctx, value_index - 1);
  duk_put_prop(ctx, obj_index);
  duk_pop(ctx);
}

void DuktapeRuntime::setPropertyValue(facebook::jsi::Object &obj,
                                      const facebook::jsi::String &key,
                                      const facebook::jsi::Value &value) {
  dukPushJsiPtrValue(ctx, std::move(obj));
  int obj_index = duk_normalize_index(ctx, -1);
  assert(duk_is_object(ctx, obj_index));
  dukPushJsiPtrValue(ctx, std::move(key));
  assert(duk_is_string(ctx, -1));
  int key_index = duk_normalize_index(ctx, -1);
  dukPushJsiValue(ctx, value);
  int value_index = duk_normalize_index(ctx, -1);
  duk_pull(ctx, key_index);
  duk_pull(ctx, value_index - 1);
  duk_put_prop(ctx, obj_index);
  duk_pop(ctx);
}

bool DuktapeRuntime::isArray(const facebook::jsi::Object &obj) const {
  duk_push_heapptr(ctx, DuktapeObject::get(obj));
  auto result = duk_is_array(ctx, -1);
  duk_pop(ctx);
  return result;
}

bool DuktapeRuntime::isFunction(const facebook::jsi::Object &obj) const {
  duk_push_heapptr(ctx, DuktapeObject::get(obj));
  assert(duk_is_object(ctx, -1));
  auto result = duk_is_function(ctx, -1);
  duk_pop(ctx);
  return result;
}

bool DuktapeRuntime::isHostFunction(const facebook::jsi::Function &func) const {
  return hostFunctions->find(DuktapeObject::get(func)) != hostFunctions->end();
}

facebook::jsi::Array
DuktapeRuntime::getPropertyNames(const facebook::jsi::Object &obj) {
  duk_push_array(ctx);
  auto arr_index = duk_normalize_index(ctx, -1);
  duk_push_heapptr(ctx, DuktapeObject::get(obj));
  duk_enum(ctx, -1, 0);
  auto enum_index = duk_normalize_index(ctx, -1);
  for (unsigned int i = 0; duk_next(ctx, enum_index, 0); i++) {
    duk_put_prop_index(ctx, arr_index, i);
  }
  duk_pop_2(ctx);
  return topOfStackToValue().getObject(*this).getArray(*this);
}

facebook::jsi::Array DuktapeRuntime::createArray(size_t length) {
  duk_push_array(ctx);
  auto arr_index = duk_normalize_index(ctx, -1);
  for (size_t i = 0; i < length; i++) {
    duk_push_undefined(ctx);
    duk_put_prop_index(ctx, arr_index, i);
  }
  return topOfStackToValue().getObject(*this).getArray(*this);
}

size_t DuktapeRuntime::size(const facebook::jsi::Array &arr) {
  duk_push_heapptr(ctx, DuktapeObject::get(arr));
  auto result = duk_get_length(ctx, -1);
  duk_pop(ctx);
  return result;
}

facebook::jsi::Value
DuktapeRuntime::getValueAtIndex(const facebook::jsi::Array &ary, size_t i) {
  duk_push_heapptr(ctx, DuktapeObject::get(ary));
  duk_push_number(ctx, i);
  duk_get_prop(ctx, -2);
  auto result = this->topOfStackToValue();
  duk_pop_2(ctx);
  return result;
}

void DuktapeRuntime::setValueAtIndexImpl(facebook::jsi::Array &ary, size_t i,
                                         const facebook::jsi::Value &value) {
  dukPushJsiPtrValue(ctx, std::move(ary));
  int obj_index = duk_normalize_index(ctx, -1);
  assert(duk_is_object(ctx, obj_index));
  assert(duk_is_array(ctx, obj_index));
  duk_push_number(ctx, i);
  assert(duk_is_number(ctx, -1));
  int key_index = duk_normalize_index(ctx, -1);
  dukPushJsiValue(ctx, value);
  int value_index = duk_normalize_index(ctx, -1);
  duk_pull(ctx, key_index);
  duk_pull(ctx, value_index - 1);
  duk_put_prop(ctx, obj_index);
}

facebook::jsi::Function DuktapeRuntime::createFunctionFromHostFunction(
    const facebook::jsi::PropNameID &name, unsigned int paramCount,
    facebook::jsi::HostFunctionType func) {
  auto hf = std::make_shared<DuktapeHostFunction>(this, func);
  duk_push_c_function(ctx, DuktapeRuntime::dukProxyFunction, DUK_VARARGS);
  assert(duk_is_object(ctx, -1));
  assert(duk_is_function(ctx, -1));
  auto funcPointer = duk_get_heapptr(ctx, -1);
  hostFunctions->emplace(funcPointer, hf);
  return DuktapeObject(funcPointer).asFunction(*this);
}

facebook::jsi::Value DuktapeRuntime::call(const facebook::jsi::Function &func,
                                          const facebook::jsi::Value &jsThis,
                                          const facebook::jsi::Value *args,
                                          size_t count) {
  dukPushJsiPtrValue(ctx, std::move(func));
  dukPushJsiValue(ctx, std::move(jsThis));
  for (unsigned int i = 0; i < count; ++i) {
    dukPushJsiValue(ctx, args[i]);
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
    return facebook::jsi::Value(
        DuktapeSymbol(duk_get_heapptr(ctx, stack_index)));
  } else if (duk_is_string(ctx, stack_index)) {
    return facebook::jsi::Value(
        DuktapeString(duk_get_heapptr(ctx, stack_index)));
  } else if (duk_is_null(ctx, stack_index)) {
    return facebook::jsi::Value(nullptr);
  } else if (duk_is_object(ctx, stack_index)) {
    return facebook::jsi::Value(
        DuktapeObject(duk_get_heapptr(ctx, stack_index)));
  } else if (duk_is_undefined(ctx, stack_index)) {
    return facebook::jsi::Value();
  }
  throw std::logic_error("unknown duktype");
}

duk_ret_t DuktapeRuntime::dukHostObjectGetProxyFunction(duk_context *ctx) {
  return dukHostObjectProxyFunction(std::string("get"), ctx);
}

duk_ret_t DuktapeRuntime::dukHostObjectSetProxyFunction(duk_context *ctx) {
  return dukHostObjectProxyFunction(std::string("set"), ctx);
}

duk_ret_t DuktapeRuntime::dukHostObjectOwnKeysProxyFunction(duk_context *ctx) {
  return dukHostObjectProxyFunction(std::string("ownKeys"), ctx);
}

duk_ret_t DuktapeRuntime::dukHostObjectProxyFunction(std::string trap,
                                                     duk_context *ctx) {
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

    dt->pushValueToStack(std::move(result));
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
        dt->pushValueToStack(result);
      } catch (std::exception &e) {
        throw facebook::jsi::JSError(*dt, e.what(), "");
      }
    } else if (trap == "set") {
      try {
        ho->set(*dt, propName, args[2]);
        dt->pushValueToStack(args[2]);
      } catch (std::exception &e) {
        throw facebook::jsi::JSError(*dt, e.what(), "");
      }
    } else {
      throw std::runtime_error("unknown trap");
    }
  }
  return 1;
}

duk_ret_t DuktapeRuntime::dukProxyFunction(duk_context *ctx) {
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
  dt->pushValueToStack(result);
  return 1;
}

void DuktapeRuntime::pushValueToStack(const facebook::jsi::Value &v) {
  dukPushJsiValue(ctx, v);
}
