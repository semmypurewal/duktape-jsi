#include "duktape-2.7.0/src/duktape.h"
#include "jsi/jsi.h"
#include "jsi/jsilib.h"
#include <map>
#include <stack>

namespace DuktapeJSI {

using namespace facebook;

class DuktapeRuntime : public jsi::Runtime {
public:
  DuktapeRuntime();
  ~DuktapeRuntime();

  jsi::Value evaluateJavaScript(const std::shared_ptr<const jsi::Buffer> &,
                                const std::string &) override;

  std::shared_ptr<const jsi::PreparedJavaScript>
  prepareJavaScript(const std::shared_ptr<const jsi::Buffer> &,
                    std::string) override;

  jsi::Value evaluatePreparedJavaScript(
      const std::shared_ptr<const jsi::PreparedJavaScript> &) override;

  bool drainMicrotasks(int) override {
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

  jsi::PropNameID createPropNameIDFromSymbol(const jsi::Symbol &) override {
    throw std::logic_error("createPropNameIDFromSymbol: unimplemented method");
  }

  std::string utf8(const jsi::PropNameID &) override;

  bool compare(const jsi::PropNameID &, const jsi::PropNameID &) override;

  std::string symbolToString(const jsi::Symbol &) override;

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

  bool strictEquals(const jsi::Symbol &, const jsi::Symbol &) const override;

  bool strictEquals(const jsi::String &, const jsi::String &) const override;

  bool strictEquals(const jsi::Object &a, const jsi::Object &b) const override;

  bool instanceOf(const jsi::Object &, const jsi::Function &) override;

  size_t getStackTop();

  std::string stackEltToString(int idx);

  std::string stackToString();

private:
  // forward declarations
  struct DuktapeScopeState;
  struct DuktapeHostFunction;
  struct DuktapeHostObject;
  template <typename T> class DuktapeWrapper;

  // typedefs
  using HostFunctionMapType =
      std::map<void *, std::shared_ptr<DuktapeRuntime::DuktapeHostFunction>>;

  using HostObjectMapType =
      std::map<void *, std::shared_ptr<DuktapeRuntime::DuktapeHostObject>>;

  // instance members
  duk_context *ctx;
  std::map<void *, size_t> *refCounts_;
  std::stack<std::shared_ptr<DuktapeScopeState>> *scopeStack_;

  // instance methods
  jsi::Value stackToValue(int);
  jsi::Value topOfStackToValue();
  template <typename T> T wrap(int stackIndex = -1);
  void *ptr(const jsi::Pointer &) const;
  size_t idx(const jsi::Pointer &) const;
  void increaseRefCount(void *);
  void decreaseRefCount(void *);
  bool hasReference(void *);
  void dukPushJsiValue(const jsi::Value &);
  template <typename T> void dukPushJsiPtrValue(T &&);
  void dukPushUtf8String(const std::string &);
  std::string dukCopyStringAsUtf8(int);
  std::shared_ptr<DuktapeScopeState> pushDuktapeScope();
  void popDuktapeScope();
  void createCppRef(jsi::Value &v);
  void throwValueOnTopOfStack();
  void throwJSError(std::string message);
  void throwJSError(std::string message, std::string stack);

  // static members
  static HostFunctionMapType *hostFunctions;
  static HostObjectMapType *hostObjects;

  // static methods
  static duk_ret_t hostFunctionProxy(duk_context *);
  static duk_ret_t hostObjectProxy(std::string, duk_context *);
  static duk_ret_t hostObjectGetProxy(duk_context *);
  static duk_ret_t hostObjectSetProxy(duk_context *);
  static duk_ret_t hostObjectOwnKeysProxy(duk_context *);

  // inner structs/classes
  struct DuktapeScopeState {
    DuktapeScopeState(DuktapeRuntime &rt)
        : rt_(rt), stackBase_(duk_get_top(rt.ctx)) {
      isValid = true;
    }

    void popUnreferenced() {
      if (isValid) {
        while (duk_normalize_index(rt_.ctx, -1) > (int)stackBase_ &&
               !rt_.hasReference(duk_get_heapptr(rt_.ctx, -1))) {
          duk_pop(rt_.ctx);
        }
      }
    }

    void pushReturnValue(const jsi::Value &result) {
      rt_.dukPushJsiValue(result);
      stackBase_ = duk_normalize_index(rt_.ctx, -1);
    }

    void invalidate() { isValid = false; }

    DuktapeRuntime &rt_;
    size_t stackBase_;
    bool isValid;
  };

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

  struct DuktapePointerValue : public jsi::Runtime::PointerValue {
    DuktapePointerValue(DuktapeRuntime &rt, int stackIndex)
        : rt_(rt), scope_(rt.scopeStack_->top()),
          dukPtr_(duk_get_heapptr(rt.ctx, stackIndex)),
          stackIndex_(stackIndex) {
      rt_.increaseRefCount(dukPtr_);
    };

    DuktapePointerValue(const DuktapePointerValue &other)
        : rt_(other.rt_), scope_(other.scope_), dukPtr_(other.dukPtr_),
          stackIndex_(other.stackIndex_) {
      rt_.increaseRefCount(dukPtr_);
    }

    static jsi::Runtime::PointerValue *
    clone(const jsi::Runtime::PointerValue *pv) {
      return new DuktapePointerValue(
          *(static_cast<const DuktapePointerValue *>(pv)));
    }

    void invalidate() override {
      rt_.decreaseRefCount(dukPtr_);
      scope_->popUnreferenced();
    };

    DuktapeRuntime &rt_;
    std::shared_ptr<DuktapeScopeState> scope_;
    void *dukPtr_;
    size_t stackIndex_;
  };

  template <typename T> class DuktapeWrapper : public T {
  public:
    DuktapeWrapper<T>(DuktapeRuntime &rt, int stackIndex)
        : T(new DuktapePointerValue(rt, stackIndex)){};

    void *ptr() const {
      return static_cast<DuktapePointerValue *>(this->ptr_)->dukPtr_;
    }
    size_t idx() const {
      auto ptr = static_cast<DuktapePointerValue *>(this->ptr_);
      auto &rt = ptr->rt_;
      if (rt.scopeStack_->top() != ptr->scope_) {
        duk_push_heapptr(rt.ctx, ptr->dukPtr_);
        return duk_normalize_index(rt.ctx, -1);
      }
      return static_cast<DuktapePointerValue *>(this->ptr_)->stackIndex_;
    }
  };
};
} // namespace DuktapeJSI
