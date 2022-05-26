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

  v = evaluateScript(*dt, "'hello world';");
  assert(v.getString(*dt).utf8(*dt) == std::string("hello world"));

  v = evaluateScript(
      *dt, "const temp = {test:'hello', test2: 42, bool_test:false}; temp;");
  auto obj = v.getObject(*dt);
  assert(obj.getProperty(*dt, "test").getString(*dt).utf8(*dt) == "hello");
  assert(obj.getProperty(*dt, "test2").getNumber() == 42);
  assert(obj.getProperty(*dt, "bool_test").getBool() == false);

  std::cout << "OK!" << std::endl;
}
