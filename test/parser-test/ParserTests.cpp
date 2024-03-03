
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
    Expr e = BinaryE(mkptr<Expr>(LiteralE{lhsTok.get()}),
                     mkptr<Expr>(LiteralE{rhsTok.get()}), tok.get());

    ExprPrinter ep;
    std::visit(ep, e);
    auto result = ep.getResult();
    CHECK_EQ("(12 - 32)", result);
  }
}

TEST_CASE("Parse expr test") {
  PARSER_TEST("plus test", R"(
1 + 2
)",
              R"((1 + 2))");

  PARSER_TEST("minus test", R"(
1 - 2
)",
              R"((1 - 2))");

  PARSER_TEST("multiply test", R"(
1 * 2
)",
              R"((1 * 2))");

  PARSER_TEST("divide test", R"(
1 / 2
)",
              R"((1 / 2))");

  PARSER_TEST("complex precedence test", R"(
1 + 2 * 3 + 4 / 5 / ( 6 - 8 ) - (-9) + !10
)",
              R"(((((1 + (2 * 3)) + ((4 / 5) / ((6 - 8)))) - (-9)) + !10))");
}

} // namespace lox