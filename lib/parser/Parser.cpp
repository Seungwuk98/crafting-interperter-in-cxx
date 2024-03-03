#include "lox/parser/Parser.hpp"
#include "llvm/Support/FormatVariadic.h"

namespace lox {

uptr<Expr> Parser::Expression() { return equality(); }

uptr<Expr> Parser::equality() {
  auto expr = comparison();
  if (!expr)
    return nullptr;

  while (match(Tok_bang_equal, Tok_equal_equal)) {
    auto opKind = previous();
    auto right = comparison();
    if (!right)
      return nullptr;
    expr = mkptr<Expr>(BinaryE(std::move(expr), std::move(right), opKind));
  }

  return expr;
}

uptr<Expr> Parser::comparison() {
  auto expr = term();
  if (!expr)
    return nullptr;

  while (match(Tok_gt, Tok_ge, Tok_lt, Tok_le)) {
    auto opKind = previous();
    auto right = term();
    if (!right)
      return nullptr;
    expr = mkptr<Expr>(BinaryE(std::move(expr), std::move(right), opKind));
  }

  return expr;
}

uptr<Expr> Parser::term() {
  auto expr = factor();
  if (!expr)
    return nullptr;

  while (match(Tok_minus, Tok_plus)) {
    auto opKind = previous();
    auto right = factor();
    if (!right)
      return nullptr;
    expr = mkptr<Expr>(BinaryE(std::move(expr), std::move(right), opKind));
  }

  return expr;
}

uptr<Expr> Parser::factor() {
  auto expr = unary();
  if (!expr)
    return nullptr;

  while (match(Tok_star, Tok_slash)) {
    auto opKind = previous();
    auto right = unary();
    if (!right)
      return nullptr;
    expr = mkptr<Expr>(BinaryE(std::move(expr), std::move(right), opKind));
  }

  return expr;
}

uptr<Expr> Parser::unary() {
  if (match(Tok_bang, Tok_minus)) {
    auto opKind = previous();
    auto expr = unary();
    if (!expr)
      return nullptr;
    return mkptr<Expr>(UnaryE(std::move(expr), opKind));
  }

  return primary();
}

uptr<Expr> Parser::primary() {
  if (match(Tok_true, Tok_false, Tok_nil, Tok_number, Tok_string))
    return mkptr<Expr>(LiteralE(previous()));

  if (match(Tok_lparen)) {
    auto expr = Expression();
    if (!match(Tok_rparen)) {
      report(peek()->Loc, SourceMgr::DK_Error,
             llvm::formatv("unexpected token. expected : {0} got : {1}",
                           getTokenName(Tok_rparen), getTokenName(peek()->Kind))
                 .str());
      return nullptr;
    }
    return mkptr<Expr>(GroupingE(std::move(expr)));
  }
  return nullptr;
}

void Parser::synchronize() {
  advance();

  while (!isAtEnd()) {
    if (previous()->Kind == Tok_semicolon)
      return;

    switch (peek()->Kind) {
    case Tok_class:
    case Tok_fun:
    case Tok_var:
    case Tok_for:
    case Tok_if:
    case Tok_while:
    case Tok_print:
    case Tok_return:
      return;
    default:
      break;
    }
    advance();
  }
}

} // namespace lox
