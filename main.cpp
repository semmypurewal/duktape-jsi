#include "duktape-jsi.h"
#include <iostream>

int main(int argc, char **argv) {
  std::cout << "hello world" << std::endl;
  DuktapeRuntime *dt = new DuktapeRuntime();
  std::cout << "dt object created!" << std::endl;
  const std::shared_ptr<const Buffer> sb =
      std::make_shared<const StringBuffer>("const temp = 5+2+20.5; temp;");
  std::cout << "sp created!" << std::endl;
  Value v = dt->evaluateJavaScript(sb, "");
  std::cout << v.getNumber() << std::endl;

  const std::shared_ptr<const Buffer> sb2 =
      std::make_shared<const StringBuffer>("const result = temp + 50; result;");
  v = dt->evaluateJavaScript(sb2, "");
  std::cout << v.getNumber() << std::endl;
}
