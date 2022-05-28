#include "duktape-jsi.h"
#include "jsi/jsi.h"
#include <cassert>

using namespace facebook;

DuktapeRuntime::DuktapeRuntime() { ctx = duk_create_heap_default(); }

facebook::jsi::Value DuktapeRuntime::evaluateJavaScript(
    const std::shared_ptr<const facebook::jsi::Buffer> &buffer,
    const std::string &sourceUrl) {
  duk_eval_string(ctx, reinterpret_cast<const char *>(buffer->data()));
  return this->topOfStackToValue();
}

std::string DuktapeRuntime::description() { return "Duktape 2.7 Runtime"; }

bool DuktapeRuntime::isInspectable() { return false; }

facebook::jsi::Object DuktapeRuntime::global() {
  duk_push_global_object(ctx);
  return DuktapeObject(duk_get_heapptr(ctx, -1));
}

facebook::jsi::Runtime::PointerValue *
DuktapeRuntime::cloneString(const facebook::jsi::Runtime::PointerValue *pv) {
  duk_push_heapptr(ctx, ((DuktapePointerValue *)pv)->duk_ptr_);
  duk_push_string(ctx, duk_get_string(ctx, -1));
  void *temp = duk_get_heapptr(ctx, -1);
  DuktapePointerValue *result = new DuktapePointerValue(temp);
  return result;
}

facebook::jsi::Runtime::PointerValue *
DuktapeRuntime::cloneObject(const facebook::jsi::Runtime::PointerValue *pv) {
  /* shallow clone for now, maybe that's okay? */
  duk_push_object(ctx);
  int clone_index = duk_normalize_index(ctx, -1);
  duk_push_heapptr(ctx, ((DuktapePointerValue *)pv)->duk_ptr_);
  duk_enum(ctx, -1, 0);
  while (duk_next(ctx, -1, 1)) {
    duk_put_prop(ctx, clone_index);
  }
  return new DuktapePointerValue(duk_get_heapptr(ctx, clone_index));
}

facebook::jsi::String DuktapeRuntime::createStringFromAscii(const char *str,
                                                            size_t length) {
  duk_push_string(ctx, str);
  return DuktapeString(duk_get_heapptr(ctx, -1));
}

facebook::jsi::Object DuktapeRuntime::createObject() {
  duk_push_object(ctx);
  return DuktapeObject(duk_get_heapptr(ctx, -1));
}

std::string DuktapeRuntime::utf8(const facebook::jsi::String &str) {
  duk_push_heapptr(ctx,
                   static_cast<const DuktapeString &>(str).getDukHeapPtr());
  return std::string(duk_get_string(ctx, -1));
}

facebook::jsi::Value
DuktapeRuntime::getProperty(const facebook::jsi::Object &obj,
                            const facebook::jsi::String &name) {
  duk_push_heapptr(ctx,
                   static_cast<const DuktapeObject &>(obj).getDukHeapPtr());
  assert(duk_is_object(ctx, -1));
  auto obj_index = duk_normalize_index(ctx, -1);
  duk_push_string(ctx, name.utf8(*this).c_str());
  assert(duk_is_object(ctx, obj_index));
  assert(duk_is_string(ctx, -1));
  duk_get_prop(ctx, obj_index);
  return topOfStackToValue();
}

void DuktapeRuntime::setPropertyValue(facebook::jsi::Object &obj,
                                      const facebook::jsi::String &key,
                                      const facebook::jsi::Value &value) {
  duk_push_jsi_ptr_value(ctx, std::move(obj));
  int obj_index = duk_normalize_index(ctx, -1);
  assert(duk_is_object(ctx, obj_index));
  duk_push_jsi_ptr_value(ctx, std::move(key));
  assert(duk_is_string(ctx, -1));
  int key_index = duk_normalize_index(ctx, -1);
  duk_push_jsi_value(ctx, value);
  int value_index = duk_normalize_index(ctx, -1);
  duk_pull(ctx, key_index);
  duk_pull(ctx, value_index - 1);
  duk_put_prop(ctx, obj_index);
}

facebook::jsi::Value DuktapeRuntime::topOfStackToValue() {
  return this->stackToValue(-1);
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
