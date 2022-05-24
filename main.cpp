#include "duktape-jsi.h"
#include <cassert>
#include <iostream>

using namespace facebook;

int main(int argc, char **argv) {
  jsi::Runtime *dt = new DuktapeRuntime();
  const std::shared_ptr<const jsi::Buffer> sb =
      std::make_shared<const jsi::StringBuffer>("const temp = 5+2+20.5; temp;");
  jsi::Value v = dt->evaluateJavaScript(sb, "");
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

  std::cout << "OK!" << std::endl;
}
