#include "duktape-jsi.h"
#include <iostream>

int main(int argc, char **argv) {
  std::cout << "hello world" << std::endl;
  DuktapeRuntime *dt = new DuktapeRuntime();
  std::cout << "dt object created!" << std::endl;
  const std::shared_ptr<const Buffer> sb =
      std::make_shared<const StringBuffer>("5+2+20.5");
  std::cout << "sp created!" << std::endl;
  Value v = dt->evaluateJavaScript(sb, "");
  std::cout << v.getNumber() << std::endl;
}
