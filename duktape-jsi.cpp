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
  return Value(topOfDukStackToValue());
}

Value DuktapeRuntime::topOfDukStackToValue() {
  if (duk_is_number(ctx, -1)) {
    return Value(duk_get_number(ctx, -1));
  } else if (duk_is_boolean(ctx, -1)) {
    return Value((bool)duk_get_boolean(ctx, -1));
  } else if (duk_is_symbol(ctx, -1)) {
    throw std::logic_error("JS Value type Symbol not implemented");
  } else if (duk_is_string(ctx, -1)) {
    return Value(
        String::createFromUtf8(*this, (const uint8_t *)duk_get_string(ctx, -1),
                               strlen(duk_get_string(ctx, -1))));
  } else if (duk_is_null(ctx, -1)) {
    return Value(nullptr);
  } else if (duk_is_object(ctx, -1)) {
    throw std::logic_error("JS Value type Object not implemented");
  }
  return Value();
}
