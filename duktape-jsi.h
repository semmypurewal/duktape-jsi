#include "duktape-2.7.0/src/duktape.h"
#include "jsi/jsi.h"
#include <iostream>
#include <variant>

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
    throw std::logic_error("prepareJavaScript: unimplemented method");
  }

  Value evaluatePreparedJavaScript(
      const std::shared_ptr<const PreparedJavaScript> &js) override {
    throw std::logic_error("evaluatePreparedJavaScript: unimplemented method");
  }

  bool drainMicrotasks(int maxMicrotasksHint) override {
    throw std::logic_error("drainMicrotasks: unimplemented method");
  }

  Object global() override {
    throw std::logic_error("global: unimplemented method");
  }

  std::string description() override {
    throw std::logic_error("description: unimplemented method");
  }

  bool isInspectable() override {
    throw std::logic_error("isInspectable: unimplemented method");
  }

  Instrumentation &instrumentation() override {
    throw std::logic_error("instrumentation: unimplemented method");
  }

  PointerValue *cloneSymbol(const Runtime::PointerValue *pv) override {
    throw std::logic_error("cloneSymbol: unimplemented method");
  }

  PointerValue *cloneString(const Runtime::PointerValue *pv) override;

  PointerValue *cloneObject(const Runtime::PointerValue *pv) override {
    throw std::logic_error("cloneObject: unimplemented method");
  }
  PointerValue *clonePropNameID(const Runtime::PointerValue *pv) override {
    throw std::logic_error("clonePropNameID: unimplemented method");
  }

  PropNameID createPropNameIDFromAscii(const char *str,
                                       size_t length) override {
    throw std::logic_error("createPropNameIDFromAscii: unimplemented method");
  }

  PropNameID createPropNameIDFromUtf8(const uint8_t *utf8,
                                      size_t length) override {
    throw std::logic_error("createPropNameIDFromUtf8: unimplemented method");
  }
  PropNameID createPropNameIDFromString(const String &str) override {
    throw std::logic_error("createPropNameIDFromString: unimplemented method");
  }
  PropNameID createPropNameIDFromSymbol(const Symbol &sym) override {
    throw std::logic_error("createPropNameIDFromSymbol: unimplemented method");
  }
  std::string utf8(const PropNameID &) override {
    throw std::logic_error("utf8: unimplemented method");
  }
  bool compare(const PropNameID &, const PropNameID &) override {
    throw std::logic_error("compare: unimplemented method");
  }

  std::string symbolToString(const Symbol &) override {
    throw std::logic_error("symbolToString: unimplemented method");
  }

  String createStringFromAscii(const char *str, size_t length) override {
    throw std::logic_error("createStringFromAscii: unimplemented method");
  }

  String createStringFromUtf8(const uint8_t *utf8, size_t length) override {
    throw std::logic_error("createStringFromUtf8: unimplemented method");
  }

  std::string utf8(const String &str) override;

  Object createObject() override {
    throw std::logic_error("createObject: unimplemented method");
  }

  Object createObject(std::shared_ptr<HostObject> ho) override {
    throw std::logic_error("createObject: unimplemented method");
  }

  std::shared_ptr<HostObject> getHostObject(const jsi::Object &) override {
    throw std::logic_error("getHostObject: unimplemented method");
  }

  HostFunctionType &getHostFunction(const jsi::Function &) override {
    throw std::logic_error("getHostFunction: unimplemented method");
  }

  Value getProperty(const Object &, const PropNameID &name) override {
    throw std::logic_error("getProperty: unimplemented method");
  }

  Value getProperty(const Object &, const String &name) override {
    throw std::logic_error("getProperty: unimplemented method");
  }

  bool hasProperty(const Object &, const PropNameID &name) override {
    throw std::logic_error("hasProperty: unimplemented method");
  }

  bool hasProperty(const Object &, const String &name) override {
    throw std::logic_error("hasProperty: unimplemented method");
  }

  void setPropertyValue(Object &, const PropNameID &name,
                        const Value &value) override {
    throw std::logic_error("setPropertyValue: unimplemented method");
  }

  void setPropertyValue(Object &, const String &name,
                        const Value &value) override {
    throw std::logic_error("setPropertyValue: unimplemented method");
  }

  bool isArray(const Object &) const override {
    throw std::logic_error("isArray: unimplemented method");
  }

  bool isArrayBuffer(const Object &) const override {
    throw std::logic_error("isArrayBuffer: unimplemented method");
  }

  bool isFunction(const Object &) const override {
    throw std::logic_error("isFunction: unimplemented method");
  }

  bool isHostObject(const jsi::Object &) const override {
    throw std::logic_error("isHostObject: unimplemented method");
  }

  bool isHostFunction(const jsi::Function &) const override {
    throw std::logic_error("isHostFunction: unimplemented method");
  }

  Array getPropertyNames(const Object &) override {
    throw std::logic_error("getPropertyNames: unimplemented method");
  }

  WeakObject createWeakObject(const Object &) override {
    throw std::logic_error("createWeakObject: unimplemented method");
  }

  Value lockWeakObject(WeakObject &) override {
    throw std::logic_error("lockWeakObject: unimplemented method");
  }

  Array createArray(size_t length) override {
    throw std::logic_error("createArray: unimplemented method");
  }

  size_t size(const Array &) override {
    throw std::logic_error("size: unimplemented method");
  }

  size_t size(const ArrayBuffer &) override {
    throw std::logic_error("size: unimplemented method");
  }

  uint8_t *data(const ArrayBuffer &) override {
    throw std::logic_error("data: unimplemented method");
  }

  Value getValueAtIndex(const Array &, size_t i) override {
    throw std::logic_error("getValueAtIndex: unimplemented method");
  }

  void setValueAtIndexImpl(Array &, size_t i, const Value &value) override {
    throw std::logic_error("setValueAtIndexImpl: unimplemented method");
  }

  Function createFunctionFromHostFunction(const PropNameID &name,
                                          unsigned int paramCount,
                                          HostFunctionType func) override {
    throw std::logic_error(
        "createFunctionFromHostFunction: unimplemented method");
  }

  Value call(const Function &, const Value &jsThis, const Value *args,
             size_t count) override {
    throw std::logic_error("call: unimplemented method");
  }

  Value callAsConstructor(const Function &, const Value *args,
                          size_t count) override {
    throw std::logic_error("callAsConstructor: unimplemented method");
  }

  bool strictEquals(const Symbol &a, const Symbol &b) const override {
    throw std::logic_error("strictEquals: unimplemented method");
  }

  bool strictEquals(const String &a, const String &b) const override {
    throw std::logic_error("strictEquals: unimplemented method");
  }

  bool strictEquals(const Object &a, const Object &b) const override {
    throw std::logic_error("strictEquals: unimplemented method");
  }

  bool instanceOf(const Object &o, const Function &f) override {
    throw std::logic_error("instanceOf: unimplemented method");
  }

  struct DuktapePointerValue : public PointerValue {
    DuktapePointerValue(void *ptr) : duk_ptr_(ptr){};
    void invalidate() override{};
    void *duk_ptr_;
  };

  template <typename T> class DuktapeValue : public T {
  public:
    DuktapeValue<T>(void *ptr) : T(new DuktapePointerValue(ptr)){};
    DuktapeValue<T>(T &&other) : T(std::move(other)){};
    void *getDukHeapPtr() const {
      DuktapePointerValue *dtv = (DuktapePointerValue *)this->ptr_;
      void *duk_heap_ptr = dtv->duk_ptr_;
      return duk_heap_ptr;
    }
  };

  using DuktapeObjectValue = DuktapeValue<Object>;
  using DuktapeSymbolValue = DuktapeValue<Symbol>;
  using DuktapeStringValue = DuktapeValue<String>;

private:
  duk_context *ctx;
  Value topOfStackToValue();
  Value stackToValue(int stack_index);

  class DuktapeObject : HostObject {
  public:
    DuktapeObject() {}

    Value get(Runtime &runtime, const PropNameID &name) override {
      return Value();
    }

    void set(Runtime &runtime, const PropNameID &name,
             const Value &value) override {}

    std::vector<PropNameID> getPropertyNames(Runtime &rt) override {
      return std::vector<PropNameID>();
    };
  };
};
