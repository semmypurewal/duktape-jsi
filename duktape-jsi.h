#include "cesu8/cesu8.h"
#include "duktape-2.7.0/src/duktape.h"
#include "jsi/jsi.h"
#include <cassert>
#include <iostream>
#include <map>

class DuktapeRuntime : public facebook::jsi::Runtime {
public:
  DuktapeRuntime();
  ~DuktapeRuntime();

  facebook::jsi::Value
  evaluateJavaScript(const std::shared_ptr<const facebook::jsi::Buffer> &,
                     const std::string &) override;

  std::shared_ptr<const facebook::jsi::PreparedJavaScript>
  prepareJavaScript(const std::shared_ptr<const facebook::jsi::Buffer> &buffer,
                    std::string sourceURL) override {
    throw std::logic_error("prepareJavaScript: unimplemented method");
  }

  facebook::jsi::Value evaluatePreparedJavaScript(
      const std::shared_ptr<const facebook::jsi::PreparedJavaScript> &js)
      override {
    throw std::logic_error("evaluatePreparedJavaScript: unimplemented method");
  }

  bool drainMicrotasks(int maxMicrotasksHint) override {
    throw std::logic_error("drainMicrotasks: unimplemented method");
  }

  facebook::jsi::Object global() override;

  std::string description() override;

  bool isInspectable() override;

  facebook::jsi::Instrumentation &instrumentation() override {
    throw std::logic_error("instrumentation: unimplemented method");
  }

  facebook::jsi::Runtime::PointerValue *
  cloneSymbol(const facebook::jsi::Runtime::PointerValue *pv) override;

  facebook::jsi::Runtime::PointerValue *
  cloneString(const facebook::jsi::Runtime::PointerValue *pv) override;

  facebook::jsi::Runtime::PointerValue *
  cloneObject(const facebook::jsi::Runtime::PointerValue *pv) override;

  facebook::jsi::Runtime::PointerValue *
  clonePropNameID(const facebook::jsi::Runtime::PointerValue *pv) override;

  facebook::jsi::PropNameID createPropNameIDFromAscii(const char *str,
                                                      size_t l) override;

  facebook::jsi::PropNameID createPropNameIDFromUtf8(const uint8_t *utf8,
                                                     size_t length) override;

  facebook::jsi::PropNameID
  createPropNameIDFromString(const facebook::jsi::String &str) override;

  facebook::jsi::PropNameID
  createPropNameIDFromSymbol(const facebook::jsi::Symbol &sym) override {
    throw std::logic_error("createPropNameIDFromSymbol: unimplemented method");
  }

  std::string utf8(const facebook::jsi::PropNameID &) override;

  bool compare(const facebook::jsi::PropNameID &,
               const facebook::jsi::PropNameID &) override;

  std::string symbolToString(const facebook::jsi::Symbol &) override {
    throw std::logic_error("symbolToString: unimplemented method");
  }

  facebook::jsi::String createStringFromAscii(const char *str,
                                              size_t length) override;

  facebook::jsi::String createStringFromUtf8(const uint8_t *utf8,
                                             size_t length) override;

  std::string utf8(const facebook::jsi::String &str) override;

  facebook::jsi::Object createObject() override;

  facebook::jsi::Object
      createObject(std::shared_ptr<facebook::jsi::HostObject>) override;

  std::shared_ptr<facebook::jsi::HostObject>
  getHostObject(const facebook::jsi::Object &obj) override;

  facebook::jsi::HostFunctionType &
  getHostFunction(const facebook::jsi::Function &) override;

  facebook::jsi::Value
  getProperty(const facebook::jsi::Object &obj,
              const facebook::jsi::PropNameID &name) override;

  facebook::jsi::Value getProperty(const facebook::jsi::Object &,
                                   const facebook::jsi::String &) override;

  bool hasProperty(const facebook::jsi::Object &,
                   const facebook::jsi::PropNameID &name) override;

  bool hasProperty(const facebook::jsi::Object &,
                   const facebook::jsi::String &name) override;

  void setPropertyValue(facebook::jsi::Object &obj,
                        const facebook::jsi::PropNameID &name,
                        const facebook::jsi::Value &value) override;

  void setPropertyValue(facebook::jsi::Object &obj,
                        const facebook::jsi::String &key,
                        const facebook::jsi::Value &value) override;

  bool isArray(const facebook::jsi::Object &obj) const override;

  bool isArrayBuffer(const facebook::jsi::Object &) const override {
    throw std::logic_error("isArrayBuffer: unimplemented method");
  }

  bool isFunction(const facebook::jsi::Object &) const override;

  bool isHostObject(const facebook::jsi::Object &obj) const override {
    return hostObjects->find(ptr(obj)) != hostObjects->end();
  }

  bool isHostFunction(const facebook::jsi::Function &func) const override;

  facebook::jsi::Array getPropertyNames(const facebook::jsi::Object &) override;

  facebook::jsi::WeakObject
  createWeakObject(const facebook::jsi::Object &) override {
    throw std::logic_error("createWeakObject: unimplemented method");
  }

  facebook::jsi::Value lockWeakObject(facebook::jsi::WeakObject &) override {
    throw std::logic_error("lockWeakObject: unimplemented method");
  }

  facebook::jsi::Array createArray(size_t length) override;

  size_t size(const facebook::jsi::Array &) override;

  size_t size(const facebook::jsi::ArrayBuffer &) override {
    throw std::logic_error("size: unimplemented method");
  }

  uint8_t *data(const facebook::jsi::ArrayBuffer &) override {
    throw std::logic_error("data: unimplemented method");
  }

  facebook::jsi::Value getValueAtIndex(const facebook::jsi::Array &ary,
                                       size_t i) override;

  void setValueAtIndexImpl(facebook::jsi::Array &ary, size_t i,
                           const facebook::jsi::Value &value) override;

  facebook::jsi::Function
  createFunctionFromHostFunction(const facebook::jsi::PropNameID &,
                                 unsigned int,
                                 facebook::jsi::HostFunctionType) override;

  facebook::jsi::Value call(const facebook::jsi::Function &,
                            const facebook::jsi::Value &,
                            const facebook::jsi::Value *, size_t) override;

  facebook::jsi::Value callAsConstructor(const facebook::jsi::Function &,
                                         const facebook::jsi::Value *args,
                                         size_t count) override {
    throw std::logic_error("callAsConstructor: unimplemented method");
  }

  bool strictEquals(const facebook::jsi::Symbol &a,
                    const facebook::jsi::Symbol &b) const override {
    throw std::logic_error("strictEquals: unimplemented method");
  }

  bool strictEquals(const facebook::jsi::String &a,
                    const facebook::jsi::String &b) const override {
    throw std::logic_error("strictEquals: unimplemented method");
  }

  bool strictEquals(const facebook::jsi::Object &a,
                    const facebook::jsi::Object &b) const override {
    throw std::logic_error("strictEquals: unimplemented method");
  }

  bool instanceOf(const facebook::jsi::Object &o,
                  const facebook::jsi::Function &f) override {
    throw std::logic_error("instanceOf: unimplemented method");
  }

private:
  duk_context *ctx;
  facebook::jsi::Value stackToValue(int stack_index);
  facebook::jsi::Value topOfStackToValue();
  static duk_ret_t hostFunctionProxy(duk_context *ctx);
  static duk_ret_t hostObjectProxy(std::string trap, duk_context *ctx);
  static duk_ret_t hostObjectGetProxy(duk_context *ctx);
  static duk_ret_t hostObjectSetProxy(duk_context *ctx);
  static duk_ret_t hostObjectOwnKeysProxy(duk_context *ctx);

  struct DuktapeHostFunction {
    DuktapeHostFunction(DuktapeRuntime *rt,
                        facebook::jsi::HostFunctionType func)
        : rt(rt), func(func){};
    DuktapeRuntime *rt;
    facebook::jsi::HostFunctionType func;
  };

  struct DuktapeHostObject {
    DuktapeHostObject(DuktapeHostObject &) = delete;
    DuktapeHostObject(DuktapeRuntime *rt,
                      std::shared_ptr<facebook::jsi::HostObject> ho)
        : rt(rt), ho(ho){};
    DuktapeRuntime *rt;
    std::shared_ptr<facebook::jsi::HostObject> ho;
  };

  using HostFunctionMapType =
      std::map<void *, std::shared_ptr<DuktapeRuntime::DuktapeHostFunction>>;
  static HostFunctionMapType *hostFunctions;

  using HostObjectMapType =
      std::map<void *, std::shared_ptr<DuktapeRuntime::DuktapeHostObject>>;
  static HostObjectMapType *hostObjects;

  struct DuktapePointerValue : public facebook::jsi::Runtime::PointerValue {
    DuktapePointerValue(duk_context *ctx, int stackIndex)
        : dukPtr_(duk_get_heapptr(ctx, stackIndex)), ctx_(ctx),
          stackIndex_(stackIndex){};

    static facebook::jsi::Runtime::PointerValue *
    clone(const facebook::jsi::Runtime::PointerValue *pv) {
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
    DuktapeWrapper<T>(T &&other) : T(std::move(other)){};

    static void *ptr(T &obj) {
      return static_cast<const DuktapeWrapper<T> &>(obj).getDukHeapPtr();
    }

    static size_t idx(T &obj) {
      return static_cast<const DuktapeWrapper<T> &>(obj).getStackIndex();
    }

  private:
    size_t getStackIndex() const {
      DuktapePointerValue *dtv = (DuktapePointerValue *)this->ptr_;
      return dtv->stackIndex_;
    }

    void *getDukHeapPtr() const {
      DuktapePointerValue *dtv = (DuktapePointerValue *)this->ptr_;
      void *duk_heap_ptr = dtv->dukPtr_;
      return duk_heap_ptr;
    }
  };

  using DuktapeObject = DuktapeWrapper<const facebook::jsi::Object>;
  using DuktapeSymbol = DuktapeWrapper<const facebook::jsi::Symbol>;
  using DuktapeString = DuktapeWrapper<const facebook::jsi::String>;
  using DuktapeFunction = DuktapeWrapper<const facebook::jsi::Function>;
  using DuktapePropNameID = DuktapeWrapper<const facebook::jsi::PropNameID>;
  using DuktapePointer = DuktapeWrapper<const facebook::jsi::Pointer>;

  template <typename T> T wrap(int stackIndex = -1) {
    auto idx = duk_normalize_index(ctx, stackIndex);
    return T(ctx, idx);
  }

  void *ptr(const facebook::jsi::Pointer &p) const {
    return DuktapePointer::ptr(p);
  }

  size_t idx(const facebook::jsi::Pointer &p) const {
    return DuktapePointer::idx(p);
  }

  void dukPushJsiValue(const facebook::jsi::Value &value) {
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
