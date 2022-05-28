#include <cassert>
#include <iostream>

#include "duktape-jsi.h"

using namespace facebook;

jsi::Value evaluateScript(jsi::Runtime &rt, const std::string script) {
  return rt.evaluateJavaScript(
      std::make_shared<const jsi::StringBuffer>(script), "");
}

int main(int argc, char **argv) {
  jsi::Runtime *dt = new DuktapeRuntime();
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

  std::cout << "OK!" << std::endl;
}
