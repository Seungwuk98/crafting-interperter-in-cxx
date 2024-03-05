#include "lox/parser/Parser.hpp"
#include "llvm/Support/FormatVariadic.h"

namespace lox {

Program Parser::Parse() {
  Program program;
  while (!isAtEnd())
    program.emplace_back(Declaration());
  return program;
}
uptr<Stmt> Parser::Declaration() {
  uptr<Stmt> stmt;
  if (match(Tok_var))
    stmt = varDeclaration();
  else
    stmt = statement();

  if (!stmt)
    synchronize();
  return stmt;
}

uptr<Stmt> Parser::varDeclaration() {
  const auto loc = previous()->Loc;
  if (peek()->Kind != (Tok_identifier)) {
    report(peek()->Loc, SourceMgr::DK_Error,
           formatv("Unexpected token. expected {0}, got {1}",
                   getTokenName(Tok_identifier), getTokenName(peek()->Kind))
               .str());
    return nullptr;
  }
  auto *tok = advance();

  uptr<Expr> expr;
  if (match(Tok_equal))
    expr = Expression();

  if (!match(Tok_semicolon)) {
    report(peek()->Loc, SourceMgr::DK_Error,
           formatv("Unexpected token. expected {0}, got {1}",
                   getTokenName(Tok_semicolon), getTokenName(peek()->Kind))
               .str());
    return nullptr;
  }
  return mkuptr<Stmt>(VarStmt(loc, tok, std::move(expr)));
}

uptr<Stmt> Parser::statement() {
  if (match(Tok_print))
    return printStmt();
  return exprStmt();
}

uptr<Expr> Parser::Expression() { return assignment(); }

uptr<Stmt> Parser::exprStmt() {
  auto expr = Expression();
  if (!expr)
    return nullptr;
  const auto loc = getLoc(*expr);
  if (!match(Tok_semicolon))
    report(loc, SourceMgr::DK_Error, "Expect `;` after value.");
  return mkuptr<Stmt>(ExprStmt(loc, std::move(expr)));
}

uptr<Stmt> Parser::printStmt() {
  auto expr = Expression();
  if (!expr)
    return nullptr;
  const auto loc = getLoc(*expr);
  if (!match(Tok_semicolon))
    report(loc, SourceMgr::DK_Error, "Expect `;` after value.");
  return mkuptr<Stmt>(PrintStmt(loc, std::move(expr)));
}

uptr<Expr> Parser::assignment() {
  auto expr = equality();
  if (!expr)
    return nullptr;

  if (match(Tok_equal)) {
    const auto *equal = previous();
    auto valueE = assignment();
    if (!valueE)
      return nullptr;

    if (auto *varE = std::get_if<VarE>(expr.get())) {
      const auto symbol = varE->getSymbol();
      return mkuptr<Expr>(AssignE(equal->Loc, symbol, std::move(valueE)));
    }
  }
  return expr;
}

uptr<Expr> Parser::equality() {
  auto expr = comparison();
  if (!expr)
    return nullptr;

  while (match(Tok_bang_equal, Tok_equal_equal)) {
    const auto opKind = previous();
    auto right = comparison();
    if (!right)
      return nullptr;
    expr = mkuptr<Expr>(
        BinaryE(opKind->Loc, std::move(expr), std::move(right), opKind));
  }

  return expr;
}

uptr<Expr> Parser::comparison() {
  auto expr = term();
  if (!expr)
    return nullptr;

  while (match(Tok_gt, Tok_ge, Tok_lt, Tok_le)) {
    const auto opKind = previous();
    auto right = term();
    if (!right)
      return nullptr;
    expr = mkuptr<Expr>(
        BinaryE(opKind->Loc, std::move(expr), std::move(right), opKind));
  }

  return expr;
}

uptr<Expr> Parser::term() {
  auto expr = factor();
  if (!expr)
    return nullptr;

  while (match(Tok_minus, Tok_plus)) {
    const auto opKind = previous();
    auto right = factor();
    if (!right)
      return nullptr;
    expr = mkuptr<Expr>(
        BinaryE(opKind->Loc, std::move(expr), std::move(right), opKind));
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
    expr = mkuptr<Expr>(
        BinaryE(opKind->Loc, std::move(expr), std::move(right), opKind));
  }

  return expr;
}

uptr<Expr> Parser::unary() {
  if (match(Tok_bang, Tok_minus)) {
    const auto opKind = previous();
    auto expr = unary();
    if (!expr)
      return nullptr;
    return mkuptr<Expr>(UnaryE(opKind->Loc, std::move(expr), opKind));
  }

  return primary();
}

uptr<Expr> Parser::primary() {
  if (match(Tok_true, Tok_false, Tok_nil, Tok_number, Tok_string))
    return mkuptr<Expr>(LiteralE(previous()->Loc, previous()));
  if (match(Tok_identifier))
    return mkuptr<Expr>(VarE(previous()->Loc, previous()));

  if (match(Tok_lparen)) {
    auto loc = previous()->Loc;
    auto expr = Expression();
    if (!match(Tok_rparen)) {
      report(peek()->Loc, SourceMgr::DK_Error,
             llvm::formatv("unexpected token. expected : {0} got : {1}",
                           getTokenName(Tok_rparen), getTokenName(peek()->Kind))
                 .str());
      return nullptr;
    }
    return mkuptr<Expr>(GroupingE(loc, std::move(expr)));
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
