#include "duktape-jsi.h"
#include <cassert>
#include <iostream>

int main(int argc, char **argv) {
  facebook::jsi::Runtime *dt = new DuktapeRuntime();
  const std::shared_ptr<const Buffer> sb =
      std::make_shared<const StringBuffer>("const temp = 5+2+20.5; temp;");
  Value v = dt->evaluateJavaScript(sb, "");
  assert(v.getNumber() == 27.5);

  const std::shared_ptr<const Buffer> sb2 =
      std::make_shared<const StringBuffer>("const result = temp + 50; result;");
  v = dt->evaluateJavaScript(sb2, "");
  assert(v.getNumber() == 77.5);

  const std::shared_ptr<const Buffer> sb3 =
      std::make_shared<const StringBuffer>("'hello world';");
  v = dt->evaluateJavaScript(sb3, "");
  assert(v.getString(*dt).utf8(*dt) == std::string("hello world"));

  std::cout << "OK!" << std::endl;
}
