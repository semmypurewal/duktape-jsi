#include "duktape-2.7.0/src/duktape.h"
#include "jsi/jsi.h"
#include <iostream>

using namespace facebook;
using namespace facebook::jsi;

class DuktapeRuntime : public Runtime {
public:
  DuktapeRuntime() {
    std::cout << "constructor called" << std::endl;
    ctx = duk_create_heap_default();
  }

  Value topOfDukStackToValue() {
    if (duk_is_number(ctx, -1)) {
      return Value(duk_get_number(ctx, -1));
    } else if (duk_is_boolean(ctx, -1)) {
      return Value((bool)duk_get_boolean(ctx, -1));
    } else if (duk_is_symbol(ctx, -1)) {
      throw std::logic_error("JS Value type Symbol not implemented");
    } else if (duk_is_string(ctx, -1)) {
      return Value(String::createFromUtf8(
          *this, (const uint8_t *)duk_get_string(ctx, -1),
          strlen(duk_get_string(ctx, -1))));
    } else if (duk_is_null(ctx, -1)) {
      return Value(nullptr);
    } else if (duk_is_object(ctx, -1)) {
      throw std::logic_error("JS Value type Object not implemented");
    }
    return Value();
  }

  Value evaluateJavaScript(const std::shared_ptr<const Buffer> &buffer,
                           const std::string &sourceUrl) {
    std::cout << "inside evaluate JavaScript" << std::endl;
    duk_eval_string(ctx, reinterpret_cast<const char *>(buffer->data()));
    // int temp = (int)duk_get_int(ctx, -1);
    // std::cout << temp << std::endl;
    return Value(topOfDukStackToValue());
  }

  std::shared_ptr<const PreparedJavaScript>
  prepareJavaScript(const std::shared_ptr<const Buffer> &buffer,
                    std::string sourceURL) {
    throw std::logic_error("unimplemented method");
  }

  Value evaluatePreparedJavaScript(
      const std::shared_ptr<const PreparedJavaScript> &js) {
    throw std::logic_error("unimplemented method");
  }

  bool drainMicrotasks(int maxMicrotasksHint) {
    throw std::logic_error("unimplemented method");
  }

  Object global() { throw std::logic_error("unimplemented method"); }

  std::string description() { throw std::logic_error("unimplemented method"); }

  bool isInspectable() { throw std::logic_error("unimplemented method"); }

  Instrumentation &instrumentation() {
    throw std::logic_error("unimplemented method");
  }

  PointerValue *cloneSymbol(const Runtime::PointerValue *pv) {
    throw std::logic_error("unimplemented method");
  }
  PointerValue *cloneString(const Runtime::PointerValue *pv) {
    throw std::logic_error("unimplemented method");
  }
  PointerValue *cloneObject(const Runtime::PointerValue *pv) {
    throw std::logic_error("unimplemented method");
  }
  PointerValue *clonePropNameID(const Runtime::PointerValue *pv) {
    throw std::logic_error("unimplemented method");
  }

  PropNameID createPropNameIDFromAscii(const char *str, size_t length) {
    throw std::logic_error("unimplemented method");
  }
  PropNameID createPropNameIDFromUtf8(const uint8_t *utf8, size_t length) {
    throw std::logic_error("unimplemented method");
  }
  PropNameID createPropNameIDFromString(const String &str) {
    throw std::logic_error("unimplemented method");
  }
  PropNameID createPropNameIDFromSymbol(const Symbol &sym) {
    throw std::logic_error("unimplemented method");
  }
  std::string utf8(const PropNameID &) {
    throw std::logic_error("unimplemented method");
  }
  bool compare(const PropNameID &, const PropNameID &) {
    throw std::logic_error("unimplemented method");
  }

  std::string symbolToString(const Symbol &) {
    throw std::logic_error("unimplemented method");
  }

  String createStringFromAscii(const char *str, size_t length) {
    throw std::logic_error("unimplemented method");
  }
  String createStringFromUtf8(const uint8_t *utf8, size_t length) {
    throw std::logic_error("unimplemented method");
  }
  std::string utf8(const String &) {
    throw std::logic_error("unimplemented method");
  }

  Object createObject() { throw std::logic_error("unimplemented method"); }
  Object createObject(std::shared_ptr<HostObject> ho) {
    throw std::logic_error("unimplemented method");
  }
  std::shared_ptr<HostObject> getHostObject(const jsi::Object &) {
    throw std::logic_error("unimplemented method");
  }
  HostFunctionType &getHostFunction(const jsi::Function &) {
    throw std::logic_error("unimplemented method");
  }

  Value getProperty(const Object &, const PropNameID &name) {
    throw std::logic_error("unimplemented method");
  }
  Value getProperty(const Object &, const String &name) {
    throw std::logic_error("unimplemented method");
  }
  bool hasProperty(const Object &, const PropNameID &name) {
    throw std::logic_error("unimplemented method");
  }
  bool hasProperty(const Object &, const String &name) {
    throw std::logic_error("unimplemented method");
  }
  void setPropertyValue(Object &, const PropNameID &name, const Value &value) {
    throw std::logic_error("unimplemented method");
  }
  void setPropertyValue(Object &, const String &name, const Value &value) {
    throw std::logic_error("unimplemented method");
  }

  bool isArray(const Object &) const {
    throw std::logic_error("unimplemented method");
  }
  bool isArrayBuffer(const Object &) const {
    throw std::logic_error("unimplemented method");
  }
  bool isFunction(const Object &) const {
    throw std::logic_error("unimplemented method");
  }
  bool isHostObject(const jsi::Object &) const {
    throw std::logic_error("unimplemented method");
  }
  bool isHostFunction(const jsi::Function &) const {
    throw std::logic_error("unimplemented method");
  }
  Array getPropertyNames(const Object &) {
    throw std::logic_error("unimplemented method");
  }

  WeakObject createWeakObject(const Object &) {
    throw std::logic_error("unimplemented method");
  }
  Value lockWeakObject(WeakObject &) {
    throw std::logic_error("unimplemented method");
  }

  Array createArray(size_t length) {
    throw std::logic_error("unimplemented method");
  }
  size_t size(const Array &) { throw std::logic_error("unimplemented method"); }
  size_t size(const ArrayBuffer &) {
    throw std::logic_error("unimplemented method");
  }
  uint8_t *data(const ArrayBuffer &) {
    throw std::logic_error("unimplemented method");
  }
  Value getValueAtIndex(const Array &, size_t i) {
    throw std::logic_error("unimplemented method");
  }
  void setValueAtIndexImpl(Array &, size_t i, const Value &value) {
    throw std::logic_error("unimplemented method");
  }

  Function createFunctionFromHostFunction(const PropNameID &name,
                                          unsigned int paramCount,
                                          HostFunctionType func) {
    throw std::logic_error("unimplemented method");
  }
  Value call(const Function &, const Value &jsThis, const Value *args,
             size_t count) {
    throw std::logic_error("unimplemented method");
  }
  Value callAsConstructor(const Function &, const Value *args, size_t count) {
    throw std::logic_error("unimplemented method");
  }
  bool strictEquals(const Symbol &a, const Symbol &b) const {
    throw std::logic_error("unimplemented method");
  }
  bool strictEquals(const String &a, const String &b) const {
    throw std::logic_error("unimplemented method");
  }
  bool strictEquals(const Object &a, const Object &b) const {
    throw std::logic_error("unimplemented method");
  }

  bool instanceOf(const Object &o, const Function &f) {
    throw std::logic_error("unimplemented method");
  }

private:
  duk_context *ctx;
};

int main(int argc, char **argv) {
  std::cout << "hello world" << std::endl;
  DuktapeRuntime *dt = new DuktapeRuntime();
  std::cout << "dt object created!" << std::endl;
  const std::shared_ptr<const Buffer> sb =
      std::make_shared<const StringBuffer>("5+2+20.5");
  std::cout << "sp created!" << std::endl;
  Value v = dt->evaluateJavaScript(sb, "");
  std::cout << v.getNumber() << std::endl;
}
