#include <cassert>
#include <iostream>

#include "duktape-jsi.h"

using namespace facebook;

int main(int argc, char **argv) {
  jsi::Runtime *dt = new DuktapeRuntime();
  jsi::Value v;

  const std::shared_ptr<const jsi::Buffer> sb =
      std::make_shared<const jsi::StringBuffer>("const temp = 5+2+20.5; temp;");
  v = dt->evaluateJavaScript(sb, "");
  assert(v.getNumber() == 27.5);

  const std::shared_ptr<const jsi::Buffer> sb2 =
      std::make_shared<const jsi::StringBuffer>(
          "const result = temp + 50; result;");
  v = dt->evaluateJavaScript(sb2, "");
  assert(v.getNumber() == 77.5);

  const std::shared_ptr<const jsi::Buffer> sb3 =
      std::make_shared<const jsi::StringBuffer>("'hello world';");
  v = dt->evaluateJavaScript(sb3, "");
  assert(v.getString(*dt).utf8(*dt) == std::string("hello world"));

  const std::shared_ptr<const jsi::Buffer> sb4 =
      std::make_shared<const jsi::StringBuffer>(
          "const temp = {test:'hello', test2: 42, bool_test:false}; temp;");
  v = dt->evaluateJavaScript(sb4, "");
  auto obj = v.getObject(*dt);
  auto temp = obj.getProperty(*dt, "test");
  assert(temp.getString(*dt).utf8(*dt) == std::string("hello"));
  temp = obj.getProperty(*dt, "test2");
  assert(temp.getNumber() == 42);
  temp = obj.getProperty(*dt, "bool_test");
  assert(temp.getBool() == false);

  std::cout << "OK!" << std::endl;
}
