#include "cesu8/cesu8.h"
#include "duktape-2.7.0/src/duktape.h"
#include "jsi/jsi.h"
#include <cassert>
#include <iostream>
#include <map>

namespace DuktapeJSI {

using namespace facebook;

class DuktapeRuntime : public jsi::Runtime {
public:
  DuktapeRuntime();
  ~DuktapeRuntime();

  jsi::Value evaluateJavaScript(const std::shared_ptr<const jsi::Buffer> &,
                                const std::string &) override;

  std::shared_ptr<const jsi::PreparedJavaScript>
  prepareJavaScript(const std::shared_ptr<const jsi::Buffer> &buffer,
                    std::string sourceURL) override {
    throw std::logic_error("prepareJavaScript: unimplemented method");
  }

  jsi::Value evaluatePreparedJavaScript(
      const std::shared_ptr<const jsi::PreparedJavaScript> &js) override {
    throw std::logic_error("evaluatePreparedJavaScript: unimplemented method");
  }

  bool drainMicrotasks(int maxMicrotasksHint) override {
    throw std::logic_error("drainMicrotasks: unimplemented method");
  }

  jsi::Object global() override;

  std::string description() override;

  bool isInspectable() override;

  jsi::Instrumentation &instrumentation() override {
    throw std::logic_error("instrumentation: unimplemented method");
  }

  jsi::Runtime::PointerValue *
  cloneSymbol(const jsi::Runtime::PointerValue *pv) override;

  jsi::Runtime::PointerValue *
  cloneString(const jsi::Runtime::PointerValue *pv) override;

  jsi::Runtime::PointerValue *
  cloneObject(const jsi::Runtime::PointerValue *pv) override;

  jsi::Runtime::PointerValue *
  clonePropNameID(const jsi::Runtime::PointerValue *pv) override;

  jsi::PropNameID createPropNameIDFromAscii(const char *str, size_t l) override;

  jsi::PropNameID createPropNameIDFromUtf8(const uint8_t *utf8,
                                           size_t length) override;

  jsi::PropNameID createPropNameIDFromString(const jsi::String &str) override;

  jsi::PropNameID createPropNameIDFromSymbol(const jsi::Symbol &sym) override {
    throw std::logic_error("createPropNameIDFromSymbol: unimplemented method");
  }

  std::string utf8(const jsi::PropNameID &) override;

  bool compare(const jsi::PropNameID &, const jsi::PropNameID &) override;

  std::string symbolToString(const jsi::Symbol &) override {
    throw std::logic_error("symbolToString: unimplemented method");
  }

  jsi::String createStringFromAscii(const char *str, size_t length) override;

  jsi::String createStringFromUtf8(const uint8_t *utf8, size_t length) override;

  std::string utf8(const jsi::String &str) override;

  jsi::Object createObject() override;

  jsi::Object createObject(std::shared_ptr<jsi::HostObject>) override;

  std::shared_ptr<jsi::HostObject>
  getHostObject(const jsi::Object &obj) override;

  jsi::HostFunctionType &getHostFunction(const jsi::Function &) override;

  jsi::Value getProperty(const jsi::Object &obj,
                         const jsi::PropNameID &name) override;

  jsi::Value getProperty(const jsi::Object &, const jsi::String &) override;

  bool hasProperty(const jsi::Object &, const jsi::PropNameID &name) override;

  bool hasProperty(const jsi::Object &, const jsi::String &name) override;

  void setPropertyValue(jsi::Object &obj, const jsi::PropNameID &name,
                        const jsi::Value &value) override;

  void setPropertyValue(jsi::Object &obj, const jsi::String &key,
                        const jsi::Value &value) override;

  bool isArray(const jsi::Object &obj) const override;

  bool isArrayBuffer(const jsi::Object &) const override {
    throw std::logic_error("isArrayBuffer: unimplemented method");
  }

  bool isFunction(const jsi::Object &) const override;

  bool isHostObject(const jsi::Object &) const override;

  bool isHostFunction(const jsi::Function &func) const override;

  jsi::Array getPropertyNames(const jsi::Object &) override;

  jsi::WeakObject createWeakObject(const jsi::Object &) override {
    throw std::logic_error("createWeakObject: unimplemented method");
  }

  jsi::Value lockWeakObject(jsi::WeakObject &) override {
    throw std::logic_error("lockWeakObject: unimplemented method");
  }

  jsi::Array createArray(size_t length) override;

  size_t size(const jsi::Array &) override;

  size_t size(const jsi::ArrayBuffer &) override {
    throw std::logic_error("size: unimplemented method");
  }

  uint8_t *data(const jsi::ArrayBuffer &) override {
    throw std::logic_error("data: unimplemented method");
  }

  jsi::Value getValueAtIndex(const jsi::Array &ary, size_t i) override;

  void setValueAtIndexImpl(jsi::Array &ary, size_t i,
                           const jsi::Value &value) override;

  jsi::Function createFunctionFromHostFunction(const jsi::PropNameID &,
                                               unsigned int,
                                               jsi::HostFunctionType) override;

  jsi::Value call(const jsi::Function &, const jsi::Value &, const jsi::Value *,
                  size_t) override;

  jsi::Value callAsConstructor(const jsi::Function &, const jsi::Value *args,
                               size_t count) override;

  bool strictEquals(const jsi::Symbol &a, const jsi::Symbol &b) const override {
    throw std::logic_error("strictEquals: unimplemented method");
  }

  bool strictEquals(const jsi::String &a, const jsi::String &b) const override {
    throw std::logic_error("strictEquals: unimplemented method");
  }

  bool strictEquals(const jsi::Object &a, const jsi::Object &b) const override {
    throw std::logic_error("strictEquals: unimplemented method");
  }

  bool instanceOf(const jsi::Object &, const jsi::Function &) override;

private:
  duk_context *ctx;
  jsi::Value stackToValue(int stack_index);
  jsi::Value topOfStackToValue();
  static duk_ret_t hostFunctionProxy(duk_context *ctx);
  static duk_ret_t hostObjectProxy(std::string trap, duk_context *ctx);
  static duk_ret_t hostObjectGetProxy(duk_context *ctx);
  static duk_ret_t hostObjectSetProxy(duk_context *ctx);
  static duk_ret_t hostObjectOwnKeysProxy(duk_context *ctx);

  struct DuktapeHostFunction {
    DuktapeHostFunction(DuktapeRuntime *rt, jsi::HostFunctionType func)
        : rt(rt), func(func){};
    DuktapeRuntime *rt;
    jsi::HostFunctionType func;
  };

  struct DuktapeHostObject {
    DuktapeHostObject(DuktapeHostObject &) = delete;
    DuktapeHostObject(DuktapeRuntime *rt, std::shared_ptr<jsi::HostObject> ho)
        : rt(rt), ho(ho){};
    DuktapeRuntime *rt;
    std::shared_ptr<jsi::HostObject> ho;
  };

  using HostFunctionMapType =
      std::map<void *, std::shared_ptr<DuktapeRuntime::DuktapeHostFunction>>;
  static HostFunctionMapType *hostFunctions;

  using HostObjectMapType =
      std::map<void *, std::shared_ptr<DuktapeRuntime::DuktapeHostObject>>;
  static HostObjectMapType *hostObjects;

  struct DuktapePointerValue : public jsi::Runtime::PointerValue {
    DuktapePointerValue(duk_context *ctx, int stackIndex)
        : dukPtr_(duk_get_heapptr(ctx, stackIndex)), ctx_(ctx),
          stackIndex_(stackIndex){};

    static jsi::Runtime::PointerValue *
    clone(const jsi::Runtime::PointerValue *pv) {
      return new DuktapePointerValue(
          *(static_cast<const DuktapePointerValue *>(pv)));
    }
    void invalidate() override{};
    void *dukPtr_;
    duk_context *ctx_;
    size_t stackIndex_;
  };

  template <typename T> class DuktapeWrapper : public T {
  public:
    DuktapeWrapper<T>(duk_context *ctx, int stackIndex)
        : T(new DuktapePointerValue(ctx, stackIndex)){};

    void *ptr() const {
      return static_cast<DuktapePointerValue *>(this->ptr_)->dukPtr_;
    }
    size_t idx() const {
      return static_cast<DuktapePointerValue *>(this->ptr_)->stackIndex_;
    }
  };

  template <typename T> T wrap(int stackIndex = -1) {
    auto idx = duk_normalize_index(ctx, stackIndex);
    return DuktapeWrapper<T>(ctx, idx);
  }

  void *ptr(const jsi::Pointer &p) const {
    return static_cast<const DuktapeWrapper<const jsi::Pointer> &>(p).ptr();
  }

  size_t idx(const jsi::Pointer &p) const {
    return static_cast<const DuktapeWrapper<const jsi::Pointer> &>(p).idx();
  }

  void dukPushJsiValue(const jsi::Value &value) {
    if (value.isUndefined()) {
      duk_push_undefined(ctx);
    } else if (value.isNull()) {
      duk_push_null(ctx);
    } else if (value.isBool()) {
      duk_push_boolean(ctx, value.getBool());
    } else if (value.isNumber()) {
      duk_push_number(ctx, value.getNumber());
    } else if (value.isString()) {
      dukPushJsiPtrValue(ctx, value.getString(*this));
    } else if (value.isSymbol()) {
      dukPushJsiPtrValue(ctx, value.getSymbol(*this));
    } else if (value.isObject()) {
      dukPushJsiPtrValue(ctx, value.getObject(*this));
    } else {
      throw std::logic_error("unknown duk type");
    }
  }

  template <typename T> void dukPushJsiPtrValue(duk_context *ctx, T &&value) {
    duk_push_heapptr(ctx, ptr(value));
  }

  void dukPushUtf8String(const std::string &utf8) {
    auto cesu8 = (char *)copy_utf8_as_cesu8(utf8.c_str(), strlen(utf8.c_str()));
    duk_push_string(ctx, cesu8);
    free(cesu8);
  }

  std::string dukCopyStringAsUtf8(int stack_index) {
    auto cesu8 = duk_get_string(ctx, stack_index);
    auto utf8 = (char *)copy_cesu8_as_utf8(cesu8, strlen(cesu8));
    auto result = std::string(utf8);
    free(utf8);
    return result;
  }
};
} // namespace DuktapeJSI
