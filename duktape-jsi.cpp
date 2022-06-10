#include "duktape-jsi.h"
#include "jsi/jsi.h"
#include <cassert>

using namespace facebook;

unsigned int DuktapeRuntime::current_hf_id = 0;
DuktapeRuntime::HostFunctionMapType *DuktapeRuntime::host_functions =
    new HostFunctionMapType;
const char *DuktapeRuntime::DUKTAPE_HOST_FUNCTION_ID_KEY =
    "___duk_host_function_id";

DuktapeRuntime::DuktapeRuntime() { ctx = duk_create_heap_default(); }
DuktapeRuntime::~DuktapeRuntime() {
  // remove host functions from global map
  std::vector<int> v;
  for (auto &pair : *host_functions) {
    if (pair.second->rt == this) {
      v.push_back(pair.first);
    }
  }

  for (auto index : v) {
    host_functions->erase(index);
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
    auto err_message = duk_safe_to_string(ctx, -1);
    throw facebook::jsi::JSError(*this, err_message);
  } else {
    return this->topOfStackToValue(ctx);
  }
}

std::string DuktapeRuntime::description() { return "Duktape 2.7 Runtime"; }

bool DuktapeRuntime::isInspectable() { return false; }

facebook::jsi::Object DuktapeRuntime::global() {
  duk_push_global_object(ctx);
  return DuktapeObject(duk_get_heapptr(ctx, -1));
}

facebook::jsi::PropNameID
DuktapeRuntime::createPropNameIDFromAscii(const char *str, size_t length) {
  duk_push_string(ctx, str);
  return DuktapePropNameID(duk_get_heapptr(ctx, -1));
}

facebook::jsi::PropNameID
DuktapeRuntime::createPropNameIDFromUtf8(const uint8_t *utf8, size_t length) {
  duk_push_string(ctx, (char *)utf8);
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
  duk_push_string(ctx, str);
  return DuktapeString(duk_get_heapptr(ctx, -1));
}

facebook::jsi::String DuktapeRuntime::createStringFromUtf8(const uint8_t *utf8,
                                                           size_t length) {
  duk_push_string(ctx, (char *)utf8);
  return DuktapeString(duk_get_heapptr(ctx, -1));
}

facebook::jsi::Object DuktapeRuntime::createObject() {
  duk_push_object(ctx);
  return DuktapeObject(duk_get_heapptr(ctx, -1));
}

std::string DuktapeRuntime::utf8(const facebook::jsi::String &str) {
  duk_push_heapptr(ctx, DuktapeString::get(str));
  return std::string(duk_get_string(ctx, -1));
}

std::string DuktapeRuntime::utf8(const facebook::jsi::PropNameID &prop) {
  duk_push_heapptr(ctx, DuktapePropNameID::get(prop));
  return std::string(duk_get_string(ctx, -1));
}

facebook::jsi::HostFunctionType &
DuktapeRuntime::getHostFunction(const facebook::jsi::Function &func) {
  duk_push_heapptr(ctx, DuktapeObject::get(func));
  duk_push_string(ctx, DuktapeRuntime::DUKTAPE_HOST_FUNCTION_ID_KEY);
  duk_get_prop(ctx, -2);
  unsigned int id = duk_get_number(ctx, -1);
  duk_pop_2(ctx);
  auto host_func = host_functions->at(id);
  return host_func->func;
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
  return topOfStackToValue(ctx);
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
  return topOfStackToValue(ctx);
}

bool DuktapeRuntime::hasProperty(const facebook::jsi::Object &obj,
                                 const facebook::jsi::PropNameID &name) {
  duk_push_heapptr(ctx, DuktapeObject::get(obj));
  return duk_has_prop_heapptr(ctx, -1, DuktapePropNameID::get(name));
}

bool DuktapeRuntime::hasProperty(const facebook::jsi::Object &obj,
                                 const facebook::jsi::String &name) {
  duk_push_heapptr(ctx, DuktapeObject::get(obj));
  return duk_has_prop_heapptr(ctx, -1, DuktapeString::get(name));
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
}

bool DuktapeRuntime::isArray(const facebook::jsi::Object &obj) const {
  duk_push_heapptr(ctx, DuktapeObject::get(obj));
  return duk_is_array(ctx, -1);
}

bool DuktapeRuntime::isFunction(const facebook::jsi::Object &obj) const {
  duk_push_heapptr(ctx, DuktapeObject::get(obj));
  assert(duk_is_object(ctx, -1));
  auto result = duk_is_function(ctx, -1);
  duk_pop(ctx);
  return result;
}

bool DuktapeRuntime::isHostFunction(const facebook::jsi::Function &func) const {
  duk_push_heapptr(ctx, DuktapeObject::get(func));
  duk_push_string(ctx, DuktapeRuntime::DUKTAPE_HOST_FUNCTION_ID_KEY);
  duk_get_prop(ctx, -2);
  auto result = duk_is_number(ctx, -1);
  duk_pop_2(ctx);
  return result;
}

facebook::jsi::Array
DuktapeRuntime::getPropertyNames(const facebook::jsi::Object &obj) {
  duk_push_array(ctx);
  auto arr_index = duk_normalize_index(ctx, -1);
  duk_push_heapptr(ctx, DuktapeObject::get(obj));
  // auto obj_index = duk_normalize_index(ctx, -1);
  duk_enum(ctx, -1, 0);
  auto enum_index = duk_normalize_index(ctx, -1);
  for (unsigned int i = 0; duk_next(ctx, enum_index, 0); i++) {
    duk_put_prop_index(ctx, arr_index, i);
  }
  return stackToValue(ctx, arr_index).getObject(*this).getArray(*this);
}

facebook::jsi::Array DuktapeRuntime::createArray(size_t length) {
  duk_push_array(ctx);
  auto arr_index = duk_normalize_index(ctx, -1);
  for (size_t i = 0; i < length; i++) {
    duk_push_undefined(ctx);
    duk_put_prop_index(ctx, arr_index, i);
  }
  return topOfStackToValue(ctx).getObject(*this).getArray(*this);
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
  auto result = this->topOfStackToValue(ctx);
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
  unsigned int id = current_hf_id++;
  host_functions->emplace(id, hf);

  duk_push_c_function(ctx, DuktapeRuntime::dukProxyFunction, DUK_VARARGS);
  duk_push_string(ctx, DuktapeRuntime::DUKTAPE_HOST_FUNCTION_ID_KEY);
  duk_push_number(ctx, id);
  duk_put_prop(ctx, -3);

  auto temp = duk_get_heapptr(ctx, -1);
  assert(duk_is_object(ctx, -1));
  assert(duk_is_function(ctx, -1));

  return DuktapeObject(temp).asFunction(*this);
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
  return topOfStackToValue(ctx);
}

facebook::jsi::Value DuktapeRuntime::topOfStackToValue(duk_context *ctx) {
  return stackToValue(ctx, -1);
}

facebook::jsi::Value DuktapeRuntime::stackToValue(duk_context *ctx,
                                                  int stack_index) {
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

duk_ret_t DuktapeRuntime::dukProxyFunction(duk_context *ctx) {
  int n = duk_get_top(ctx);
  std::vector<facebook::jsi::Value> args;

  for (int i = 0; i < n; ++i) {
    args.push_back(DuktapeRuntime::stackToValue(ctx, i - n));
  }
  for (int i = 0; i < n; ++i) {
    duk_pop(ctx);
  }

  duk_push_current_function(ctx);
  duk_push_string(ctx, DuktapeRuntime::DUKTAPE_HOST_FUNCTION_ID_KEY);
  duk_get_prop(ctx, -2);
  unsigned int id = duk_get_number(ctx, -1);
  duk_pop_2(ctx);
  auto host_func = host_functions->at(id);
  auto dt = host_func->rt;
  auto func = host_func->func;
  auto result = func(*dt, facebook::jsi::Value(), args.data(), n);
  dt->pushValueToStack(result);
  return 1;
}

void DuktapeRuntime::pushValueToStack(const facebook::jsi::Value &v) {
  dukPushJsiValue(ctx, v);
}
