
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "ParserTestUtils.hpp"
#include "doctest/doctest.h"
#include "lox/ast/ASTPrinter.hpp"

TEST_SUITE("Parser tests") { TEST_CASE("Entrypoint"){}; }

namespace lox {

TEST_CASE("AST Print test") {
  SUBCASE("binary expr print") {
    auto tok = mkptr<Token>(SMLoc(), 0u, TokenKind::Tok_minus, "-");
    auto lhsTok = mkptr<Token>(SMLoc(), 0u, TokenKind::Tok_number, "12");
    auto rhsTok = mkptr<Token>(SMLoc(), 0u, TokenKind::Tok_number, "32");
    Expr e = BinaryE({}, mkptr<Expr>(LiteralE{{}, lhsTok.get()}),
                     mkptr<Expr>(LiteralE{{}, rhsTok.get()}), tok.get());

    std::string result;
    ExprPrinter ep(result);
    std::visit(ep, e);
    CHECK_EQ("(12 - 32)", result);
  }
}

TEST_CASE("Parse expr test") {
  EXPR_PARSE_TEST("plus test", R"(
1 + 2
)",
                  R"((1 + 2))");

  EXPR_PARSE_TEST("minus test", R"(
1 - 2
)",
                  R"((1 - 2))");

  EXPR_PARSE_TEST("multiply test", R"(
1 * 2
)",
                  R"((1 * 2))");

  EXPR_PARSE_TEST("divide test", R"(
1 / 2
)",
                  R"((1 / 2))");

  EXPR_PARSE_TEST(
      "complex precedence test", R"(
1 + 2 * 3 + 4 / 5 / ( 6 - 8 ) - (-9) + !10
)",
      R"(((((1 + (2 * 3)) + ((4 / 5) / ((6 - 8)))) - (-9)) + !10))");

  EXPR_PARSE_TEST("string add", R"(
"abcde" + "fgh"
)",
                  R"(("abcde" + "fgh"))");

  PROGRAM_PARSE_TEST("print", R"(
print "ABCDE";
)",
                     R"(print "ABCDE";
)");

  PROGRAM_PARSE_TEST("exprs",
                     R"(
1 + 2;
2 + 3 + 4;
print 1 + 2 * 3;
)",
                     R"((1 + 2);
((2 + 3) + 4);
print (1 + (2 * 3));
)");
}

} // namespace lox