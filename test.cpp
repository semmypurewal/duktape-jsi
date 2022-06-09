#include "duktape-jsi.h"
#include "gtest/gtest.h"
#include <iostream>

using ::testing::InitGoogleTest;
using ::testing::Test;

class DuktapeRuntimeTest : public Test {
protected:
  std::shared_ptr<facebook::jsi::Runtime> dt;

  void SetUp() { dt = std::make_shared<DuktapeRuntime>(); }
  void TearDown() {}

  facebook::jsi::Value evaluateScript(const std::string script) {
    return dt->evaluateJavaScript(
        std::make_shared<const facebook::jsi::StringBuffer>(script), "");
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

TEST_F(DuktapeRuntimeTest, MaintainStateBetweenEvaluateCalls) {
  evaluateScript("const temp = 5+2+20.5; temp;");
  auto v = evaluateScript("const result = temp + 50; result;");
  EXPECT_EQ(v.getNumber(), 77.5);
}

TEST_F(DuktapeRuntimeTest, StringValueReturn) {
  auto v = evaluateScript("'hello from JavaScript!';");
  EXPECT_EQ(v.getString(*dt).utf8(*dt), std::string("hello from JavaScript!"));
}

TEST_F(DuktapeRuntimeTest, ObjectValueReturn) {
  auto v = evaluateScript(
      "const temp = {test:'hello', test2: 42, bool_test:false}; temp;");
  auto obj = v.getObject(*dt);
  EXPECT_EQ(obj.getProperty(*dt, "test").getString(*dt).utf8(*dt), "hello");
  EXPECT_EQ(
      obj.getProperty(*dt, facebook::jsi::PropNameID::forAscii(*dt, "test2"))
          .getNumber(),
      42);
  EXPECT_EQ(obj.getProperty(*dt, "bool_test").getBool(), false);

  obj.setProperty(*dt, "num_test", 24);
  obj.setProperty(*dt, facebook::jsi::PropNameID::forAscii(*dt, "KEY"),
                  "VALUE");
  obj.setProperty(*dt, "bool_test", true);
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
      [&](facebook::jsi::Runtime &rt, const facebook::jsi::Value &thisVal,
          const facebook::jsi::Value *args,
          size_t count) { return facebook::jsi::Value(capture); };

  auto v = evaluateHostFunctionFromJS(std::string("func_with_captures"),
                                      std::string("func_with_captures();"), 0,
                                      func_with_captures);
  ASSERT_TRUE(v.isNumber());
  EXPECT_EQ(v.getNumber(), capture);
}

TEST_F(DuktapeRuntimeTest, HostFunctionWithNumberArgs) {
  facebook::jsi::HostFunctionType func_with_num_args =
      [](facebook::jsi::Runtime &rt, const facebook::jsi::Value &thisVal,
         const facebook::jsi::Value *args, size_t count) {
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
      [](facebook::jsi::Runtime &rt, const facebook::jsi::Value &thisVal,
         const facebook::jsi::Value *args, size_t count) {
        bool result = false;
        for (int i = 0; i < count; ++i) {
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
      [](facebook::jsi::Runtime &rt, const facebook::jsi::Value &thisVal,
         const facebook::jsi::Value *args, size_t count) {
        assert(args[0].isString());
        assert(args[1].isNumber());
        auto str = args[0].getString(rt).utf8(rt);
        int index = args[1].getNumber();

        if (0 <= index && index < str.length()) {
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

int main(int argc, char **argv) {
  InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
