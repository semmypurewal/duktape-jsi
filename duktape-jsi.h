#include "duktape-2.7.0/src/duktape.h"
#include "jsi/jsi.h"
#include <iostream>

using namespace facebook;
using namespace facebook::jsi;

class DuktapeRuntime : public Runtime {
public:
  DuktapeRuntime();

  Value evaluateJavaScript(const std::shared_ptr<const Buffer> &,
                           const std::string &) override;

  std::shared_ptr<const PreparedJavaScript>
  prepareJavaScript(const std::shared_ptr<const Buffer> &buffer,
                    std::string sourceURL) override {
    throw std::logic_error("unimplemented method");
  }

  Value evaluatePreparedJavaScript(
      const std::shared_ptr<const PreparedJavaScript> &js) override {
    throw std::logic_error("unimplemented method");
  }

  bool drainMicrotasks(int maxMicrotasksHint) override {
    throw std::logic_error("unimplemented method");
  }

  Object global() override { throw std::logic_error("unimplemented method"); }

  std::string description() override {
    throw std::logic_error("unimplemented method");
  }

  bool isInspectable() override {
    throw std::logic_error("unimplemented method");
  }

  Instrumentation &instrumentation() override {
    throw std::logic_error("unimplemented method");
  }

  PointerValue *cloneSymbol(const Runtime::PointerValue *pv) override {
    throw std::logic_error("unimplemented method");
  }
  PointerValue *cloneString(const Runtime::PointerValue *pv) override {
    throw std::logic_error("unimplemented method");
  }
  PointerValue *cloneObject(const Runtime::PointerValue *pv) override {
    throw std::logic_error("unimplemented method");
  }
  PointerValue *clonePropNameID(const Runtime::PointerValue *pv) override {
    throw std::logic_error("unimplemented method");
  }

  PropNameID createPropNameIDFromAscii(const char *str,
                                       size_t length) override {
    throw std::logic_error("unimplemented method");
  }
  PropNameID createPropNameIDFromUtf8(const uint8_t *utf8,
                                      size_t length) override {
    throw std::logic_error("unimplemented method");
  }
  PropNameID createPropNameIDFromString(const String &str) override {
    throw std::logic_error("unimplemented method");
  }
  PropNameID createPropNameIDFromSymbol(const Symbol &sym) override {
    throw std::logic_error("unimplemented method");
  }
  std::string utf8(const PropNameID &) override {
    throw std::logic_error("unimplemented method");
  }
  bool compare(const PropNameID &, const PropNameID &) override {
    throw std::logic_error("unimplemented method");
  }

  std::string symbolToString(const Symbol &) override {
    throw std::logic_error("unimplemented method");
  }

  String createStringFromAscii(const char *str, size_t length) override {
    throw std::logic_error("unimplemented method");
  }

  String createStringFromUtf8(const uint8_t *utf8, size_t length) override {
    throw std::logic_error("unimplemented method");
  }

  std::string utf8(const String &) override {
    throw std::logic_error("unimplemented method");
  }

  Object createObject() override {
    throw std::logic_error("unimplemented method");
  }

  Object createObject(std::shared_ptr<HostObject> ho) override {
    throw std::logic_error("unimplemented method");
  }

  std::shared_ptr<HostObject> getHostObject(const jsi::Object &) override {
    throw std::logic_error("unimplemented method");
  }

  HostFunctionType &getHostFunction(const jsi::Function &) override {
    throw std::logic_error("unimplemented method");
  }

  Value getProperty(const Object &, const PropNameID &name) override {
    throw std::logic_error("unimplemented method");
  }

  Value getProperty(const Object &, const String &name) override {
    throw std::logic_error("unimplemented method");
  }

  bool hasProperty(const Object &, const PropNameID &name) override {
    throw std::logic_error("unimplemented method");
  }

  bool hasProperty(const Object &, const String &name) override {
    throw std::logic_error("unimplemented method");
  }

  void setPropertyValue(Object &, const PropNameID &name,
                        const Value &value) override {
    throw std::logic_error("unimplemented method");
  }

  void setPropertyValue(Object &, const String &name,
                        const Value &value) override {
    throw std::logic_error("unimplemented method");
  }

  bool isArray(const Object &) const override {
    throw std::logic_error("unimplemented method");
  }

  bool isArrayBuffer(const Object &) const override {
    throw std::logic_error("unimplemented method");
  }

  bool isFunction(const Object &) const override {
    throw std::logic_error("unimplemented method");
  }

  bool isHostObject(const jsi::Object &) const override {
    throw std::logic_error("unimplemented method");
  }

  bool isHostFunction(const jsi::Function &) const override {
    throw std::logic_error("unimplemented method");
  }

  Array getPropertyNames(const Object &) override {
    throw std::logic_error("unimplemented method");
  }

  WeakObject createWeakObject(const Object &) override {
    throw std::logic_error("unimplemented method");
  }

  Value lockWeakObject(WeakObject &) override {
    throw std::logic_error("unimplemented method");
  }

  Array createArray(size_t length) override {
    throw std::logic_error("unimplemented method");
  }

  size_t size(const Array &) override {
    throw std::logic_error("unimplemented method");
  }

  size_t size(const ArrayBuffer &) override {
    throw std::logic_error("unimplemented method");
  }

  uint8_t *data(const ArrayBuffer &) override {
    throw std::logic_error("unimplemented method");
  }

  Value getValueAtIndex(const Array &, size_t i) override {
    throw std::logic_error("unimplemented method");
  }

  void setValueAtIndexImpl(Array &, size_t i, const Value &value) override {
    throw std::logic_error("unimplemented method");
  }

  Function createFunctionFromHostFunction(const PropNameID &name,
                                          unsigned int paramCount,
                                          HostFunctionType func) override {
    throw std::logic_error("unimplemented method");
  }

  Value call(const Function &, const Value &jsThis, const Value *args,
             size_t count) override {
    throw std::logic_error("unimplemented method");
  }

  Value callAsConstructor(const Function &, const Value *args,
                          size_t count) override {
    throw std::logic_error("unimplemented method");
  }

  bool strictEquals(const Symbol &a, const Symbol &b) const override {
    throw std::logic_error("unimplemented method");
  }

  bool strictEquals(const String &a, const String &b) const override {
    throw std::logic_error("unimplemented method");
  }

  bool strictEquals(const Object &a, const Object &b) const override {
    throw std::logic_error("unimplemented method");
  }

  bool instanceOf(const Object &o, const Function &f) override {
    throw std::logic_error("unimplemented method");
  }

private:
  duk_context *ctx;
  Value topOfDukStackToValue();
};
