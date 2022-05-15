#include "duktape-jsi.h"
#include "jsi/jsi.h"

using namespace facebook;
using namespace facebook::jsi;

DuktapeRuntime::DuktapeRuntime() {
  std::cout << "constructor called" << std::endl;
  ctx = duk_create_heap_default();
}
