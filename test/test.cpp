#include "duktape-jsi.h"
#include "gtest/gtest.h"

using ::testing::InitGoogleTest;
using ::testing::Test;

using namespace DuktapeJSI;

class DuktapeRuntimeTest : public Test {
protected:
  std::shared_ptr<facebook::jsi::Runtime> dt;
  std::shared_ptr<DuktapeRuntime> debugRt;

  void SetUp() {
    debugRt = std::make_shared<DuktapeRuntime>();
    dt = debugRt;
  }
  void TearDown() {}

  facebook::jsi::Value eval(const char *code) {
    return dt->global().getPropertyAsFunction(*dt, "eval").call(*dt, code);
  }

  facebook::jsi::Value evaluateScript(const std::string script) {
    return dt->evaluateJavaScript(
        std::make_shared<const facebook::jsi::StringBuffer>(script), "");
  }

  facebook::jsi::Function function(const std::string &code) {
    return eval(std::string("(" + code + ")").c_str())
        .getObject(*dt)
        .getFunction(*dt);
  }

  facebook::jsi::Value
  evaluateHostFunctionFromJS(std::string &&js_name, std::string &&js_code,
                             int paramCount,
                             facebook::jsi::HostFunctionType func) {

    auto f = facebook::jsi::Function::createFromHostFunction(
        *dt,
        facebook::jsi::PropNameID::forAscii(*dt, js_name.c_str(),
                                            js_name.length()),
        paramCount, func);
    dt->global().setProperty(*dt, js_name.c_str(), f);
    return evaluateScript(js_code);
  }
};

TEST_F(DuktapeRuntimeTest, BasicEvaluateJS) {
  auto v = evaluateScript("const temp = 5+2+20.5; temp;");
  EXPECT_EQ(v.getNumber(), 27.5);
}

TEST_F(DuktapeRuntimeTest, StackManagement) {
  EXPECT_TRUE(eval("undefined").isUndefined());
  EXPECT_TRUE(eval("null").isNull());
  EXPECT_TRUE(eval("true").isBool());
  EXPECT_TRUE(eval("false").isBool());
  EXPECT_TRUE(eval("123").isNumber());
  EXPECT_TRUE(eval("123.4").isNumber());
  EXPECT_TRUE(eval("'str'").isString());
  EXPECT_TRUE(eval("({})").isObject());
  EXPECT_TRUE(eval("[]").isObject());
  EXPECT_TRUE(eval("(function(){})").isObject());

  EXPECT_EQ(eval("123").getNumber(), 123);
  EXPECT_EQ(eval("123.4").getNumber(), 123.4);
  EXPECT_EQ(eval("'str'").getString(*dt).utf8(*dt), "str");
  EXPECT_TRUE(eval("[]").getObject(*dt).isArray(*dt));

  EXPECT_TRUE(eval("undefined").isUndefined());
  EXPECT_TRUE(eval("null").isNull());
  EXPECT_TRUE(eval("true").isBool());
  EXPECT_TRUE(eval("false").isBool());
  EXPECT_TRUE(eval("123").isNumber());
  EXPECT_TRUE(eval("123.4").isNumber());
  EXPECT_TRUE(eval("'str'").isString());
  EXPECT_TRUE(eval("({})").isObject());
  EXPECT_TRUE(eval("[]").isObject());
  EXPECT_TRUE(eval("(function(){})").isObject());

  EXPECT_TRUE(eval("true").asBool());
  EXPECT_THROW(eval("123").asBool(), facebook::jsi::JSIException);
  EXPECT_EQ(eval("456").asNumber(), 456);
  EXPECT_THROW(eval("'word'").asNumber(), facebook::jsi::JSIException);
  EXPECT_EQ(
      eval("({1:2, 3:4})").asObject(*dt).getProperty(*dt, "1").getNumber(), 2);
  EXPECT_THROW(eval("'oops'").asObject(*dt), facebook::jsi::JSIException);

  EXPECT_EQ(eval("['zero',1,2,3]").toString(*dt).utf8(*dt), "zero,1,2,3");

  EXPECT_EQ(eval("123").getNumber(), 123);
  EXPECT_EQ(eval("123.4").getNumber(), 123.4);
  EXPECT_EQ(eval("'str'").getString(*dt).utf8(*dt), "str");
  EXPECT_TRUE(eval("[]").getObject(*dt).isArray(*dt));
  // next expectation is currently wrong due to the way we handle
  // scopes that might return a value. It should be fixable though.
  // EXPECT_EQ(debugRt->getStackTop(), 0);
}

TEST_F(DuktapeRuntimeTest, MaintainStateBetweenEvaluateCalls) {
  evaluateScript("const temp = 5+2+20.5; temp;");
  auto v = evaluateScript("const result = temp + 50; result;");
  EXPECT_EQ(v.getNumber(), 77.5);
}

TEST_F(DuktapeRuntimeTest, SyntaxError) {
  EXPECT_THROW(evaluateScript("syntax error="), facebook::jsi::JSError);
}

TEST_F(DuktapeRuntimeTest, StringValueReturn) {
  auto v = evaluateScript("'hello from JavaScript!';");
  EXPECT_EQ(v.getString(*dt).utf8(*dt), std::string("hello from JavaScript!"));
}

TEST_F(DuktapeRuntimeTest, ObjectValueReturn) {
  auto v = evaluateScript(
      "const temp = {test:'hello', test2: 42, bool_test:false}; temp;");
  auto obj = v.getObject(*dt);
  ASSERT_TRUE(obj.getPropertyNames(*dt).isArray(*dt));
  EXPECT_EQ(obj.getPropertyNames(*dt).length(*dt), 3u);
  EXPECT_EQ(obj.getProperty(*dt, "test").getString(*dt).utf8(*dt), "hello");
  EXPECT_EQ(
      obj.getProperty(*dt, facebook::jsi::PropNameID::forAscii(*dt, "test2"))
          .getNumber(),
      42);
  EXPECT_EQ(obj.getProperty(*dt, "bool_test").getBool(), false);

  obj.setProperty(*dt, "num_test", 24);
  EXPECT_EQ(obj.getPropertyNames(*dt).length(*dt), 4u);
  obj.setProperty(*dt, facebook::jsi::PropNameID::forAscii(*dt, "KEY"),
                  "VALUE");
  EXPECT_EQ(obj.getPropertyNames(*dt).length(*dt), 5u);
  obj.setProperty(*dt, "bool_test", true);
  EXPECT_TRUE(obj.hasProperty(*dt, "num_test"));
  EXPECT_TRUE(
      obj.hasProperty(*dt, facebook::jsi::PropNameID::forAscii(*dt, "KEY")));
  EXPECT_EQ(obj.getProperty(*dt, "num_test").getNumber(), 24);
  EXPECT_EQ(obj.getProperty(*dt, "KEY").getString(*dt).utf8(*dt), "VALUE");
  EXPECT_EQ(obj.getProperty(*dt, "bool_test").getBool(), true);
}

TEST_F(DuktapeRuntimeTest, ArrayValueReturn) {
  auto v = evaluateScript("var ary = ['a','b','c']; ary;");
  assert(v.isObject());
  auto ary = v.getObject(*dt).getArray(*dt);
  EXPECT_EQ(ary.getValueAtIndex(*dt, 1).getString(*dt).utf8(*dt), "b");
  EXPECT_EQ(ary.getValueAtIndex(*dt, 2).getString(*dt).utf8(*dt), "c");
  ASSERT_TRUE(ary.getValueAtIndex(*dt, 3).isUndefined());
  ary.setValueAtIndex(*dt, 3, "d");
  EXPECT_EQ(ary.getValueAtIndex(*dt, 3).getString(*dt).utf8(*dt), "d");
  ary.setValueAtIndex(*dt, 2, "e");
  EXPECT_EQ(ary.getValueAtIndex(*dt, 2).getString(*dt).utf8(*dt), "e");
}

TEST_F(DuktapeRuntimeTest, CreateObjectAndArray) {
  auto arr = facebook::jsi::Array(*dt, 5);
  EXPECT_EQ(arr.length(*dt), 5u);
  for (unsigned int i = 0; i < arr.length(*dt); i++) {
    EXPECT_TRUE(arr.getValueAtIndex(*dt, i).isUndefined());
  }

  auto obj = facebook::jsi::Object(*dt);
  EXPECT_EQ(obj.getPropertyNames(*dt).length(*dt), 0u);
}

TEST_F(DuktapeRuntimeTest, GlobalObject) {
  auto global = dt->global();
  global.setProperty(*dt, "dukMsg", "hello from C++!");
  auto v = evaluateScript("dukMsg;");
  ASSERT_TRUE(v.isString());
  EXPECT_EQ(v.getString(*dt).utf8(*dt), "hello from C++!");
}

TEST_F(DuktapeRuntimeTest, HostFunctionWithCaptures) {
  int capture = 4;
  facebook::jsi::HostFunctionType func_with_captures =
      [&](facebook::jsi::Runtime &, const facebook::jsi::Value &,
          const facebook::jsi::Value *,
          size_t) { return facebook::jsi::Value(capture); };

  auto v = evaluateHostFunctionFromJS(std::string("func_with_captures"),
                                      std::string("func_with_captures();"), 0,
                                      func_with_captures);
  ASSERT_TRUE(v.isNumber());
  EXPECT_EQ(v.getNumber(), capture);
}

TEST_F(DuktapeRuntimeTest, HostFunctionWithNumberArgs) {
  facebook::jsi::HostFunctionType func_with_num_args =
      [](facebook::jsi::Runtime &, const facebook::jsi::Value &,
         const facebook::jsi::Value *args, size_t) {
        const facebook::jsi::Value &arg1 = args[0];
        const facebook::jsi::Value &arg2 = args[1];
        assert(arg1.isNumber());
        assert(arg2.isNumber());
        return arg1.getNumber() + arg2.getNumber();
      };
  auto v = evaluateHostFunctionFromJS(
      std::string("func_with_num_args"),
      std::string("func_with_num_args(25,30.5);"), 2, func_with_num_args);
  EXPECT_TRUE(v.isNumber());
  EXPECT_EQ(v.getNumber(), 55.5);
}

TEST_F(DuktapeRuntimeTest, HostFunctionWithBooleanVarArgs) {
  facebook::jsi::HostFunctionType func_with_bool_varargs =
      [](facebook::jsi::Runtime &, const facebook::jsi::Value &,
         const facebook::jsi::Value *args, size_t count) {
        bool result = false;
        for (unsigned int i = 0; i < count; ++i) {
          assert(args[i].isBool());
          if (args[i].getBool() == true) {
            result = true;
          }
        }
        return result;
      };

  auto v = evaluateHostFunctionFromJS(
      std::string("func_with_bool_varargs"),
      std::string("arr = [];"
                  "arr.push(func_with_bool_varargs(false,false,false,true));"
                  "arr.push(func_with_bool_varargs(false,true));"
                  "arr.push(func_with_bool_varargs(false));"
                  "arr.push(func_with_bool_varargs());"
                  "arr;"),
      0, func_with_bool_varargs);

  ASSERT_TRUE(v.isObject());
  auto arr = v.getObject(*dt).getArray(*dt);
  auto arr_elt = arr.getValueAtIndex(*dt, 0);
  ASSERT_TRUE(arr_elt.isBool());
  EXPECT_EQ(arr_elt.getBool(), true);
  arr_elt = arr.getValueAtIndex(*dt, 1);
  ASSERT_TRUE(arr_elt.isBool());
  EXPECT_EQ(arr_elt.getBool(), true);
  arr_elt = arr.getValueAtIndex(*dt, 2);
  ASSERT_TRUE(arr_elt.isBool());
  EXPECT_EQ(arr_elt.getBool(), false);
  arr_elt = arr.getValueAtIndex(*dt, 3);
  ASSERT_TRUE(arr_elt.isBool());
  EXPECT_EQ(arr_elt.getBool(), false);
}

TEST_F(DuktapeRuntimeTest, HostFunctionWithHeterogeneousArgs) {
  facebook::jsi::HostFunctionType func_with_heterogeneous_args =
      [](facebook::jsi::Runtime &rt, const facebook::jsi::Value &,
         const facebook::jsi::Value *args, size_t) {
        assert(args[0].isString());
        assert(args[1].isNumber());
        auto str = args[0].getString(rt).utf8(rt);
        unsigned int index = args[1].getNumber();

        if (index < str.length()) {
          const auto result = std::string(1, str.at(index));
          return facebook::jsi::Value(
              facebook::jsi::String::createFromAscii(rt, result.c_str()));
        } else {
          return facebook::jsi::Value();
        }
      };

  auto v = evaluateHostFunctionFromJS(
      std::string("func_with_heterogeneous_args"),
      std::string("arr = [];"
                  "arr.push(func_with_heterogeneous_args('hello world', 3));"
                  "arr.push(func_with_heterogeneous_args('hello world', -1));"
                  "arr.push(func_with_heterogeneous_args('hello world', 300));"
                  "arr;"),
      2, func_with_heterogeneous_args);

  ASSERT_TRUE(v.isObject());
  auto arr = v.getObject(*dt).getArray(*dt);
  auto arr_elt = arr.getValueAtIndex(*dt, 0);
  ASSERT_TRUE(arr_elt.isString());
  EXPECT_EQ(arr_elt.getString(*dt).utf8(*dt), "l");
  arr_elt = arr.getValueAtIndex(*dt, 1);
  EXPECT_TRUE(arr_elt.isUndefined());
  arr_elt = arr.getValueAtIndex(*dt, 2);
  EXPECT_TRUE(arr_elt.isUndefined());
}

TEST_F(DuktapeRuntimeTest, HostFunctionUtilities) {
  auto host_function = [](facebook::jsi::Runtime &,
                          const facebook::jsi::Value &,
                          const facebook::jsi::Value *,
                          unsigned long) { return facebook::jsi::Value(); };

  facebook::jsi::Function f = facebook::jsi::Function::createFromHostFunction(
      *dt,
      facebook::jsi::PropNameID::forAscii(*dt, "js test", strlen("js test")), 0,
      host_function);

  ASSERT_TRUE(f.isHostFunction(*dt));
  // need to figure out how to test this one
  // EXPECT_EQ(f.getHostFunction(*dt), host_function);
}

TEST_F(DuktapeRuntimeTest, Cesu8ToUtf8Test) {
  auto eval = dt->global().getPropertyAsFunction(*dt, "eval");
  eval.call(*dt, "x = {thumbsDown:'\\uD83D\\uDC4E', "
                 "thumbsUp:'\\uD83D\\uDC4D', ok:'\\uD83C\\uDD97'};");

  facebook::jsi::Object x = dt->global().getPropertyAsObject(*dt, "x");

  auto ok = x.getProperty(*dt, "ok").getString(*dt);
  const char okUtf8[] = {(char)0xF0, (char)0x9F, (char)0x86, (char)0x97};
  EXPECT_EQ(ok.utf8(*dt), std::string(okUtf8, 4));

  auto thumbsUp = x.getProperty(*dt, "thumbsUp").getString(*dt);
  const char thumbsUpUtf8[] = {(char)0xF0, (char)0x9F, (char)0x91, (char)0x8D};
  EXPECT_EQ(thumbsUp.utf8(*dt), std::string(thumbsUpUtf8, 4));

  auto thumbsDown = x.getProperty(*dt, "thumbsDown").getString(*dt);
  const char thumbsDownUtf8[] = {(char)0xF0, (char)0x9F, (char)0x91,
                                 (char)0x8E};
  EXPECT_EQ(thumbsDown.utf8(*dt), std::string(thumbsDownUtf8, 4));
}

TEST_F(DuktapeRuntimeTest, Utf8ToCesu8Test) {
  auto eval = dt->global().getPropertyAsFunction(*dt, "eval");
  eval.call(*dt, "x = {};");

  facebook::jsi::Object x = dt->global().getPropertyAsObject(*dt, "x");

  uint8_t okUtf8[] = {0xF0, 0x9F, 0x86, 0x97};
  auto okPropName =
      facebook::jsi::String::createFromUtf8(*dt, okUtf8, sizeof(okUtf8));
  x.setProperty(*dt, facebook::jsi::PropNameID::forString(*dt, okPropName),
                "emoji1");

  EXPECT_EQ(x.getPropertyNames(*dt).size(*dt), 1u);
  EXPECT_TRUE(eval.call(*dt, "x['\\uD83C\\uDD97'] == 'emoji1'").getBool());

  uint8_t thumbsUpUtf8[] = {0xF0, 0x9F, 0x91, 0x8D};
  auto thumbsUpPropName = facebook::jsi::String::createFromUtf8(
      *dt, thumbsUpUtf8, sizeof(thumbsUpUtf8));
  x.setProperty(*dt,
                facebook::jsi::PropNameID::forString(*dt, thumbsUpPropName),
                "emoji2");

  EXPECT_EQ(x.getPropertyNames(*dt).size(*dt), 2u);
  EXPECT_TRUE(eval.call(*dt, "x['\\uD83D\\uDC4D'] == 'emoji2'").getBool());

  uint8_t thumbsDownUtf8[] = {0xF0, 0x9F, 0x91, 0x8E};
  auto thumbsDownPropName = facebook::jsi::String::createFromUtf8(
      *dt, thumbsDownUtf8, sizeof(thumbsDownUtf8));
  x.setProperty(*dt,
                facebook::jsi::PropNameID::forString(*dt, thumbsDownPropName),
                "emoji3");

  EXPECT_EQ(x.getPropertyNames(*dt).size(*dt), 3u);
  EXPECT_TRUE(eval.call(*dt, "x['\\uD83D\\uDC4E'] == 'emoji3'").getBool());
}

TEST_F(DuktapeRuntimeTest, HostObject) {
  class ConstantHostObject : public facebook::jsi::HostObject {
  public:
    bool setCalled = false;
    bool getCalled = false;
    bool getPropertyNamesCalled = false;

    facebook::jsi::Value get(facebook::jsi::Runtime &,
                             const facebook::jsi::PropNameID &) override {
      getCalled = true;
      return 9000;
    }

    void set(facebook::jsi::Runtime &, const facebook::jsi::PropNameID &,
             const facebook::jsi::Value &) override {
      setCalled = true;
    }

    std::vector<facebook::jsi::PropNameID>
    getPropertyNames(facebook::jsi::Runtime &rt) override {
      getPropertyNamesCalled = true;
      return facebook::jsi::PropNameID::names(rt, "a_prop", "1", "false",
                                              "a_prop", "3", "c_prop");
    }
  };

  auto ho = std::make_shared<ConstantHostObject>();

  facebook::jsi::Object cho =
      facebook::jsi::Object::createFromHostObject(*dt, ho);

  EXPECT_TRUE(cho.isHostObject(*dt));

  EXPECT_TRUE(function("function (obj) { return obj.someRandomProp == 9000; }")
                  .call(*dt, cho)
                  .getBool());

  EXPECT_TRUE(ho->getCalled);

  EXPECT_EQ(
      function("function (obj) { return (obj.thisIsAThing = 'randoString');}")
          .call(*dt, cho)
          .getString(*dt)
          .utf8(*dt),
      "randoString");

  EXPECT_TRUE(function("function (obj) { return "
                       "Object.getOwnPropertyNames(obj).length == 5}")
                  .call(*dt, cho)
                  .getBool());

  EXPECT_TRUE(ho->setCalled);

  EXPECT_TRUE(cho.getHostObject<ConstantHostObject>(*dt).get() != nullptr);
  EXPECT_TRUE(ho->getPropertyNamesCalled);
}

TEST_F(DuktapeRuntimeTest, HostFunctionWithError) {
  jsi::Function dot = jsi::Function::createFromHostFunction(
      *dt, jsi::PropNameID::forAscii(*dt, "dot"), 2,
      [](jsi::Runtime &rt, const jsi::Value &thisVal, const jsi::Value *args,
         size_t count) {
        EXPECT_TRUE(jsi::Value::strictEquals(rt, thisVal, rt.global()) ||
                    thisVal.isUndefined());
        if (count != 2) {
          throw std::runtime_error("expected 2 args");
        }
        std::string ret = args[0].getString(rt).utf8(rt) + "." +
                          args[1].getString(rt).utf8(rt);
        return jsi::String::createFromUtf8(
            rt, reinterpret_cast<const uint8_t *>(ret.data()), ret.size());
      });

  dt->global().setProperty(*dt, "cons", dot);

  EXPECT_TRUE(eval("(function() {"
                   "  try {"
                   "    cons('fail', 'this'); return true;"
                   "  } catch (e) {"
                   "    return false;"
                   "  }})()")
                  .getBool());

  EXPECT_TRUE(eval("(function() {"
                   "  try {"
                   "    cons('fail'); return false;"
                   "  } catch (e) {"
                   "    return ((e instanceof Error) &&"
                   "            (e.message == 'Exception in HostFunction: ' +"
                   "                          'expected 2 args'));"
                   "  }})()")
                  .getBool());
}

TEST_F(DuktapeRuntimeTest, HostObjectWithValueMembers) {
  class Bag : public jsi::HostObject {
  public:
    Bag() = default;

    const jsi::Value &operator[](const std::string &name) const {
      auto iter = data_.find(name);
      if (iter == data_.end()) {
        return undef_;
      }
      return iter->second;
    }

  protected:
    jsi::Value get(jsi::Runtime &rt, const jsi::PropNameID &name) override {
      return jsi::Value(rt, (*this)[name.utf8(rt)]);
    }

    void set(jsi::Runtime &rt, const jsi::PropNameID &name,
             const jsi::Value &val) override {
      data_.emplace(name.utf8(rt), jsi::Value(rt, val));
    }

    jsi::Value undef_;
    std::map<std::string, jsi::Value> data_;
  };

  auto sharedBag = std::make_shared<Bag>();
  auto &bag = *sharedBag;
  jsi::Object jsbag =
      jsi::Object::createFromHostObject(*dt, std::move(sharedBag));

  auto set = function("function (o) {"
                      "  o.foo = 'bar';"
                      "  o.obj = { 'foo': 'bar' };"
                      "  o.bool = true;"
                      "}");

  set.call(*dt, jsbag);
  auto checkObj =
      function("function(o) { return typeof o.obj === 'object';  }");
  auto checkStr =
      function("function(o) { return typeof o.foo === 'string';  }");
  auto checkBool =
      function("function(o) { return typeof o.bool === 'boolean';  }");

  EXPECT_TRUE(checkObj.call(*dt, jsbag).getBool());
  EXPECT_TRUE(checkStr.call(*dt, jsbag).getBool());
  EXPECT_TRUE(checkBool.call(*dt, jsbag).getBool());

  EXPECT_TRUE(bag["foo"].getString(*dt).utf8(*dt) == "bar");
  EXPECT_TRUE(bag["bool"].getBool());
  EXPECT_EQ(bag["obj"]
                .getObject(*dt)
                .getProperty(*dt, "foo")
                .getString(*dt)
                .utf8(*dt),
            "bar");
}

int main(int argc, char **argv) {
  InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
