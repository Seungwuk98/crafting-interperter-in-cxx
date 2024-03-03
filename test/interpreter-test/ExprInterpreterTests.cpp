#include "InterpreterTestUtils.hpp"
#include "doctest/doctest.h"

namespace lox {

TEST_CASE("Expression Interpreter Test" *
          doctest::test_suite("Interpreter tests")) {
  INTERPRETER_TEST("plus test", R"(10 + 20)", NumberValue(30.L));

  INTERPRETER_TEST("minus test", R"(10 - 20)", NumberValue(-10.L));

  INTERPRETER_TEST("multiply test", R"(2 * 3)", NumberValue(6.L));

  INTERPRETER_TEST("divide test", R"(2 / 5)", NumberValue(2.L / 5.L));

  INTERPRETER_TEST("string merge test", R"("abcd" + "ef")",
                   StringValue("abcdef"));

  INTERPRETER_TEST("complex operation test", R"(1 + 30 / 5 * 8 + 2 - 30 * 3)",
                   NumberValue(1.L + 30.L / 5.L * 8.L + 2.L - 30.L * 3.L));

  INTERPRETER_TEST("compare test", R"(3.0 > 2.9)", BoolValue(true));

  INTERPRETER_TEST("compare test - 2", R"(3.0 <= 2.9)", BoolValue(false));

  INTERPRETER_TEST("compare test - 3", R"(3.1 < 15.001)", BoolValue(true));

  INTERPRETER_TEST("compare test - 4", R"(4.001 >= 4.001)", BoolValue(true));

  INTERPRETER_TEST("grouping test", R"((1 + 3) * 7)",
                   NumberValue((1.L + 3.L) * 7.L));

  INTERPRETER_TEST("unary test", R"(-10 + 20)", NumberValue(10.L));

  INTERPRETER_TEST("unary test - 2", R"(!(10.4 < 10.5))", BoolValue(false));

  INTERPRETER_TEST("truthy test", R"(!("a" + "b"))", BoolValue(true));

  INTERPRETER_TEST("truthy test - 2", R"(!(10 + 30))", BoolValue(true));
}

} // namespace lox