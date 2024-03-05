#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "InterpreterTestUtils.hpp"
#include "doctest/doctest.h"

namespace lox {

TEST_SUITE("Interpreter tests") {
  TEST_CASE("Entrypoint") {}
}

TEST_CASE("Print test" * doctest::test_suite("Interpreter tests")) {
  PRINT_OUTPUT_TEST("print number", R"(
print 1 + 2;
)",
                    R"(3.000000
)");

  PRINT_OUTPUT_TEST("print string", R"(
print "hello world";
)",
                    R"(hello world
)");

  PRINT_OUTPUT_TEST("print several expression", R"(
print "hello";
print "world";
print "!";
)",
                    R"(hello
world
!
)");

  PRINT_OUTPUT_TEST("print variable expression", R"(
var a = 10;
print a;
)",
                    R"(10.000000
)");

  PRINT_OUTPUT_TEST("print variable and assign expression", R"(
var a = 10;
a = a + 10;
print a;
)", R"(20.000000
)");
}

} // namespace lox
