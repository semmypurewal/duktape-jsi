#include "duktape-jsi.h"
#include "jsi/jsi.h"

using namespace facebook;
using namespace facebook::jsi;

DuktapeRuntime::DuktapeRuntime() {
  std::cout << "constructor called" << std::endl;
  ctx = duk_create_heap_default();
}

Value DuktapeRuntime::evaluateJavaScript(
    const std::shared_ptr<const Buffer> &buffer, const std::string &sourceUrl) {
  std::cout << "inside evaluate JavaScript" << std::endl;
  duk_eval_string(ctx, reinterpret_cast<const char *>(buffer->data()));
  return this->topOfStackToValue();
}

Runtime::PointerValue *
DuktapeRuntime::cloneString(const Runtime::PointerValue *pv) {
  duk_push_heapptr(ctx, ((DuktapePointerValue *)pv)->duk_ptr_);
  duk_push_string(ctx, duk_get_string(ctx, -1));
  void *temp = duk_get_heapptr(ctx, -1);
  DuktapePointerValue *result = new DuktapePointerValue(temp);
  return result;
}

std::string DuktapeRuntime::utf8(const String &str) {
  duk_push_heapptr(
      ctx, static_cast<const DuktapeStringValue &>(str).getDukHeapPtr());
  return std::string(duk_get_string(ctx, -1));
}

Value DuktapeRuntime::topOfStackToValue() { return this->stackToValue(-1); }

Value DuktapeRuntime::stackToValue(int stack_index) {
  if (duk_is_number(ctx, stack_index)) {
    return Value(duk_get_number(ctx, stack_index));
  } else if (duk_is_boolean(ctx, stack_index)) {
    return Value((bool)duk_get_boolean(ctx, stack_index));
  } else if (duk_is_symbol(ctx, stack_index)) {
    return Value(DuktapeSymbolValue(duk_get_heapptr(ctx, stack_index)));
  } else if (duk_is_string(ctx, stack_index)) {
    return Value(DuktapeStringValue(duk_get_heapptr(ctx, stack_index)));
  } else if (duk_is_null(ctx, stack_index)) {
    return Value(nullptr);
  } else if (duk_is_object(ctx, stack_index)) {
    return Value(DuktapeObjectValue(duk_get_heapptr(ctx, stack_index)));
  } else if (duk_is_undefined(ctx, stack_index)) {
    return Value();
  }
  throw std::logic_error("unknown duktype");
}
