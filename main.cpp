#include <cassert>
#include <iostream>
#include <sstream>

#include "duktape-jsi.h"

using namespace facebook;

jsi::Value evaluateScript(jsi::Runtime &rt, const std::string script) {
  return rt.evaluateJavaScript(
      std::make_shared<const jsi::StringBuffer>(script), "");
}

jsi::Value evaluateHostFunctionFromJS(jsi::Runtime &rt, std::string &&js_name,
                                      std::string &&js_code, int paramCount,
                                      jsi::HostFunctionType func) {

  jsi::Function f = jsi::Function::createFromHostFunction(
      rt, jsi::PropNameID::forAscii(rt, js_name.c_str(), js_name.length()),
      paramCount, func);
  rt.global().setProperty(rt, js_name.c_str(), f);
  return evaluateScript(rt, js_code);
}

int main(int argc, char **argv) {
  std::shared_ptr<jsi::Runtime> dt = std::make_shared<DuktapeRuntime>();
  jsi::Value v;

  v = evaluateScript(*dt, "const temp = 5+2+20.5; temp;");
  assert(v.getNumber() == 27.5);

  v = evaluateScript(*dt, "const result = temp + 50; result;");
  assert(v.getNumber() == 77.5);

  v = evaluateScript(*dt, "'hello from JavaScript!';");
  assert(v.getString(*dt).utf8(*dt) == std::string("hello from JavaScript!"));

  v = evaluateScript(
      *dt, "const temp = {test:'hello', test2: 42, bool_test:false}; temp;");
  auto obj = v.getObject(*dt);
  assert(obj.getProperty(*dt, "test").getString(*dt).utf8(*dt) == "hello");
  assert(obj.getProperty(*dt, "test2").getNumber() == 42);
  assert(obj.getProperty(*dt, "bool_test").getBool() == false);

  obj.setProperty(*dt, "num_test", 24);
  obj.setProperty(*dt, "KEY", "VALUE");
  obj.setProperty(*dt, "bool_test", true);
  assert(obj.getProperty(*dt, "num_test").getNumber() == 24);
  assert(obj.getProperty(*dt, "KEY").getString(*dt).utf8(*dt) == "VALUE");
  assert(obj.getProperty(*dt, "bool_test").getBool() == true);

  v = evaluateScript(*dt, "var ary = ['a','b','c']; ary;");
  auto ary = v.getObject(*dt);
  assert(ary.getProperty(*dt, "1").getString(*dt).utf8(*dt) == "b");
  assert(ary.getProperty(*dt, "2").getString(*dt).utf8(*dt) == "c");
  assert(ary.getProperty(*dt, "3").isUndefined());

  auto global = dt->global();
  global.setProperty(*dt, "dukMsg", "hello from C++!");
  v = evaluateScript(*dt, "dukMsg;");
  assert(v.isString());
  assert(v.getString(*dt).utf8(*dt) == "hello from C++!");

  int capture = 4;
  jsi::HostFunctionType func_with_captures =
      [&](jsi::Runtime &rt, const jsi::Value &thisVal, const jsi::Value *args,
          size_t count) { return jsi::Value(capture); };

  v = evaluateHostFunctionFromJS(*dt, std::string("func_with_captures"),
                                 std::string("func_with_captures();"), 0,
                                 func_with_captures);
  assert(v.isNumber() && v.getNumber() == capture);

  jsi::HostFunctionType func_with_num_args =
      [](jsi::Runtime &rt, const jsi::Value &thisVal, const jsi::Value *args,
         size_t count) {
        const jsi::Value &arg1 = args[0];
        const jsi::Value &arg2 = args[1];
        assert(arg1.isNumber());
        assert(arg2.isNumber());
        return arg1.getNumber() + arg2.getNumber();
      };
  v = evaluateHostFunctionFromJS(*dt, std::string("func_with_num_args"),
                                 std::string("func_with_num_args(25,30.5);"), 2,
                                 func_with_num_args);
  assert(v.isNumber() && v.getNumber() == 55.5);

  jsi::HostFunctionType func_with_bool_varargs =
      [](jsi::Runtime &rt, const jsi::Value &thisVal, const jsi::Value *args,
         size_t count) {
        bool result = false;
        for (int i = 0; i < count; ++i) {
          assert(args[i].isBool());
          if (args[i].getBool() == true) {
            result = true;
          }
        }
        return result;
      };

  v = evaluateHostFunctionFromJS(
      *dt, std::string("func_with_bool_varargs"),
      std::string("arr = [];"
                  "arr.push(func_with_bool_varargs(false,false,false,true));"
                  "arr.push(func_with_bool_varargs(false,true));"
                  "arr.push(func_with_bool_varargs(false));"
                  "arr.push(func_with_bool_varargs());"
                  "arr;"),
      0, func_with_bool_varargs);

  assert(v.isObject());
  auto arr = v.getObject(*dt);
  auto arr_elt = arr.getProperty(*dt, "0");
  assert(arr_elt.isBool() && arr_elt.getBool() == true);
  arr_elt = arr.getProperty(*dt, "1");
  assert(arr_elt.isBool() && arr_elt.getBool() == true);
  arr_elt = arr.getProperty(*dt, "2");
  assert(arr_elt.isBool() && arr_elt.getBool() == false);
  arr_elt = arr.getProperty(*dt, "3");
  assert(arr_elt.isBool() && arr_elt.getBool() == false);

  jsi::HostFunctionType func_with_heterogeneous_args =
      [](jsi::Runtime &rt, const jsi::Value &thisVal, const jsi::Value *args,
         size_t count) {
        assert(args[0].isString());
        assert(args[1].isNumber());
        auto str = args[0].getString(rt).utf8(rt);
        int index = args[1].getNumber();

        if (0 <= index && index < str.length()) {
          const auto result = std::string(1, str.at(index));
          return jsi::Value(jsi::String::createFromAscii(rt, result.c_str()));
        } else {
          return jsi::Value();
        }
      };

  v = evaluateHostFunctionFromJS(
      *dt, std::string("func_with_heterogeneous_args"),
      std::string("arr = [];"
                  "arr.push(func_with_heterogeneous_args('hello world', 3));"
                  "arr.push(func_with_heterogeneous_args('hello world', -1));"
                  "arr.push(func_with_heterogeneous_args('hello world', 300));"
                  "arr;"),
      2, func_with_heterogeneous_args);

  assert(v.isObject());
  arr = v.getObject(*dt);
  arr_elt = arr.getProperty(*dt, "0");
  assert(arr_elt.isString());
  assert(arr_elt.isString() && arr_elt.getString(*dt).utf8(*dt) == "l");
  arr_elt = arr.getProperty(*dt, "1");
  assert(arr_elt.isUndefined());
  arr_elt = arr.getProperty(*dt, "2");
  assert(arr_elt.isUndefined());

  auto host_function = [](facebook::jsi::Runtime &,
                          const facebook::jsi::Value &,
                          const facebook::jsi::Value *,
                          unsigned long) { return facebook::jsi::Value(); };

  jsi::Function f = jsi::Function::createFromHostFunction(
      *dt, jsi::PropNameID::forAscii(*dt, "js test", strlen("js test")), 0,
      host_function);

  assert(f.isHostFunction(*dt));
  // need to figure out how to test this one
  // assert(f.getHostFunction(*dt) == host_function);

  std::cout << "OK!" << std::endl;
}
