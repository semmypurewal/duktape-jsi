#include "duktape-jsi.h"
#include "jsi/test/testlib.h"
#include "gtest/gtest.h"

using ::testing::InitGoogleTest;
using ::testing::Test;

namespace facebook {
namespace jsi {

std::vector<facebook::jsi::RuntimeFactory> runtimeGenerators() {
  return {[]() -> std::unique_ptr<facebook::jsi::Runtime> {
    return std::make_unique<DuktapeRuntime>();
  }};
}
} // namespace jsi
} // namespace facebook

int main(int argc, char **argv) {
  InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
