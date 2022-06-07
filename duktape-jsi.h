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
  cloneSymbol(const facebook::jsi::Runtime::PointerValue *pv) override {
    throw std::logic_error("cloneSymbol: unimplemented method");
  }

  facebook::jsi::Runtime::PointerValue *
  cloneString(const facebook::jsi::Runtime::PointerValue *pv) override;

  facebook::jsi::Runtime::PointerValue *
  cloneObject(const facebook::jsi::Runtime::PointerValue *pv) override;

  facebook::jsi::Runtime::PointerValue *
  clonePropNameID(const facebook::jsi::Runtime::PointerValue *pv) override {
    throw std::logic_error("clonePropNameID: unimplemented method");
  }

  facebook::jsi::PropNameID createPropNameIDFromAscii(const char *str,
                                                      size_t l) override;

  facebook::jsi::PropNameID createPropNameIDFromUtf8(const uint8_t *utf8,
                                                     size_t length) override {
    throw std::logic_error("createPropNameIDFromUtf8: unimplemented method");
  }
  facebook::jsi::PropNameID
  createPropNameIDFromString(const facebook::jsi::String &str) override {
    throw std::logic_error("createPropNameIDFromString: unimplemented method");
  }
  facebook::jsi::PropNameID
  createPropNameIDFromSymbol(const facebook::jsi::Symbol &sym) override {
    throw std::logic_error("createPropNameIDFromSymbol: unimplemented method");
  }
  std::string utf8(const facebook::jsi::PropNameID &) override {
    throw std::logic_error("utf8: unimplemented method");
  }
  bool compare(const facebook::jsi::PropNameID &,
               const facebook::jsi::PropNameID &) override {
    throw std::logic_error("compare: unimplemented method");
  }

  std::string symbolToString(const facebook::jsi::Symbol &) override {
    throw std::logic_error("symbolToString: unimplemented method");
  }

  facebook::jsi::String createStringFromAscii(const char *str,
                                              size_t length) override;

  facebook::jsi::String createStringFromUtf8(const uint8_t *utf8,
                                             size_t length) override {
    throw std::logic_error("createStringFromUtf8: unimplemented method");
  }

  std::string utf8(const facebook::jsi::String &str) override;

  facebook::jsi::Object createObject() override;

  facebook::jsi::Object
  createObject(std::shared_ptr<facebook::jsi::HostObject> ho) override {
    throw std::logic_error("createObject: unimplemented method");
  }

  std::shared_ptr<facebook::jsi::HostObject>
  getHostObject(const facebook::jsi::Object &) override {
    throw std::logic_error("getHostObject: unimplemented method");
  }

  facebook::jsi::HostFunctionType &
  getHostFunction(const facebook::jsi::Function &) override {
    throw std::logic_error("getHostFunction: unimplemented method");
  }

  facebook::jsi::Value
  getProperty(const facebook::jsi::Object &obj,
              const facebook::jsi::PropNameID &name) override {
    throw std::logic_error("getProperty: unimplemented method");
  }

  facebook::jsi::Value getProperty(const facebook::jsi::Object &,
                                   const facebook::jsi::String &) override;

  bool hasProperty(const facebook::jsi::Object &,
                   const facebook::jsi::PropNameID &name) override {
    throw std::logic_error("hasProperty: unimplemented method");
  }

  bool hasProperty(const facebook::jsi::Object &,
                   const facebook::jsi::String &name) override {
    throw std::logic_error("hasProperty: unimplemented method");
  }

  void setPropertyValue(facebook::jsi::Object &,
                        const facebook::jsi::PropNameID &name,
                        const facebook::jsi::Value &value) override {
    throw std::logic_error("setPropertyValue: unimplemented method");
  }

  void setPropertyValue(facebook::jsi::Object &obj,
                        const facebook::jsi::String &key,
                        const facebook::jsi::Value &value) override;

  bool isArray(const facebook::jsi::Object &) const override {
    throw std::logic_error("isArray: unimplemented method");
  }

  bool isArrayBuffer(const facebook::jsi::Object &) const override {
    throw std::logic_error("isArrayBuffer: unimplemented method");
  }

  bool isFunction(const facebook::jsi::Object &) const override;

  bool isHostObject(const facebook::jsi::Object &) const override {
    throw std::logic_error("isHostObject: unimplemented method");
  }

  bool isHostFunction(const facebook::jsi::Function &) const override {
    throw std::logic_error("isHostFunction: unimplemented method");
  }

  facebook::jsi::Array
  getPropertyNames(const facebook::jsi::Object &) override {
    throw std::logic_error("getPropertyNames: unimplemented method");
  }

  facebook::jsi::WeakObject
  createWeakObject(const facebook::jsi::Object &) override {
    throw std::logic_error("createWeakObject: unimplemented method");
  }

  facebook::jsi::Value lockWeakObject(facebook::jsi::WeakObject &) override {
    throw std::logic_error("lockWeakObject: unimplemented method");
  }

  facebook::jsi::Array createArray(size_t length) override {
    throw std::logic_error("createArray: unimplemented method");
  }

  size_t size(const facebook::jsi::Array &) override {
    throw std::logic_error("size: unimplemented method");
  }

  size_t size(const facebook::jsi::ArrayBuffer &) override {
    throw std::logic_error("size: unimplemented method");
  }

  uint8_t *data(const facebook::jsi::ArrayBuffer &) override {
    throw std::logic_error("data: unimplemented method");
  }

  facebook::jsi::Value getValueAtIndex(const facebook::jsi::Array &,
                                       size_t i) override {
    throw std::logic_error("getValueAtIndex: unimplemented method");
  }

  void setValueAtIndexImpl(facebook::jsi::Array &, size_t i,
                           const facebook::jsi::Value &value) override {
    throw std::logic_error("setValueAtIndexImpl: unimplemented method");
  }

  facebook::jsi::Function
  createFunctionFromHostFunction(const facebook::jsi::PropNameID &,
                                 unsigned int,
                                 facebook::jsi::HostFunctionType) override;

  facebook::jsi::Value call(const facebook::jsi::Function &,
                            const facebook::jsi::Value &jsThis,
                            const facebook::jsi::Value *args,
                            size_t count) override {
    throw std::logic_error("call: unimplemented method");
  }

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
  static unsigned int current_hf_id;
  static facebook::jsi::Value stackToValue(duk_context *ctx, int stack_index);
  static facebook::jsi::Value topOfStackToValue(duk_context *ctx);
  static duk_ret_t dukProxyFunction(duk_context *ctx);
  void pushValueToStack(facebook::jsi::Value &v);

  struct DuktapeHostFunction {
    DuktapeHostFunction(DuktapeRuntime *rt,
                        facebook::jsi::HostFunctionType func)
        : rt(rt), func(func){};
    DuktapeRuntime *rt;
    facebook::jsi::HostFunctionType func;
  };

  using HostFunctionMapType =
      std::map<int, std::shared_ptr<DuktapeRuntime::DuktapeHostFunction>>;
  static HostFunctionMapType *host_functions;

  class DuktapeHostObject : facebook::jsi::HostObject {
  public:
    DuktapeHostObject() {}

    facebook::jsi::Value get(Runtime &runtime,
                             const facebook::jsi::PropNameID &name) override {
      return facebook::jsi::Value();
    }

    void set(Runtime &runtime, const facebook::jsi::PropNameID &name,
             const facebook::jsi::Value &value) override {}

    std::vector<facebook::jsi::PropNameID>
    getPropertyNames(Runtime &rt) override {
      return std::vector<facebook::jsi::PropNameID>();
    };
  };

  struct DuktapePointerValue : public facebook::jsi::Runtime::PointerValue {
    DuktapePointerValue(void *ptr)
        : duk_ptr_(ptr){
              // hide this object from garbage collection
          };
    void invalidate() override{
        // release this object for garbage collection
    };
    void *duk_ptr_;
  };

  template <typename T> class DuktapePointer : public T {
  public:
    DuktapePointer<T>(void *ptr) : T(new DuktapePointerValue(ptr)){};
    DuktapePointer<T>(T &&other) : T(std::move(other)){};

    static void *get(T &obj) {
      return static_cast<const DuktapePointer<T> &>(obj).getDukHeapPtr();
    }

  private:
    void *getDukHeapPtr() const {
      DuktapePointerValue *dtv = (DuktapePointerValue *)this->ptr_;
      void *duk_heap_ptr = dtv->duk_ptr_;
      return duk_heap_ptr;
    }
  };

  using DuktapeObject = DuktapePointer<const facebook::jsi::Object>;
  using DuktapeSymbol = DuktapePointer<const facebook::jsi::Symbol>;
  using DuktapeString = DuktapePointer<const facebook::jsi::String>;
  using DuktapePropNameID = DuktapePointer<const facebook::jsi::PropNameID>;

  void dukPushJsiValue(duk_context *ctx, const facebook::jsi::Value &value) {
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
    duk_push_heapptr(ctx, DuktapePointer<T>::get(value));
  }
};
