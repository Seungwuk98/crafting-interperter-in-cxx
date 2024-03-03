#ifndef __LOX_PARSER_HPP__
#define __LOX_PARSER_HPP__

#include "lox/ast/AST.hpp"
#include "lox/parser/Token.hpp"
#include "utils/TypeUtils.hpp"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

namespace lox {
using namespace llvm;

class Parser {
public:
  Parser(const ArrayRef<uptr<Token>> &tokens, SourceMgr &srcMgr)
      : SrcMgr(srcMgr), errorNum(0u), tokens(tokens), cursor(0u){};

  SourceMgr &SrcMgr;

  Program Parse();

  /// Statement -> expression statement | print statement
  uptr<Stmt> Statement();

  /// expression -> equality
  uptr<Expr> Expression();

  [[nodiscard]] std::size_t getError() const { return errorNum; }

private:
  /// expression statement -> expression ';'
  uptr<Stmt> exprStmt();

  /// print statement -> "print" expr ';'
  uptr<Stmt> printStmt();

  /// equality -> comparison ( ( "!=" | "==" ) comparison )*
  uptr<Expr> equality();

  /// comparison -> term ((">" | ">=" | "<" | "<=" ) term )*
  uptr<Expr> comparison();

  /// term -> factor ( ( "-" | "+" ) factor )*
  uptr<Expr> term();

  /// factor -> unary ( ( "*" | "/" ) unary) *
  uptr<Expr> factor();

  /// unary -> ( "!" | "-" ) unary
  ///         | primary
  uptr<Expr> unary();

  /// primary -> NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")"
  uptr<Expr> primary();

  void synchronize();

  [[nodiscard]] Token *peek() const { return tokens[cursor].get(); }

  Token *advance() {
    if (!isAtEnd())
      cursor++;
    return previous();
  }

  [[nodiscard]] Token *previous() const { return tokens[cursor - 1].get(); }

  [[nodiscard]] bool isAtEnd() const {
    return peek()->Kind == TokenKind::Tok_eof;
  }

  [[nodiscard]] bool check(const TokenKind kind) const {
    if (isAtEnd())
      return false;
    return peek()->Kind == kind;
  }

  template <typename T, typename... Ts>
  inline bool match(T kind, Ts... tokenKinds) {
    if (check(kind)) {
      advance();
      return true;
    }
    if constexpr (sizeof...(Ts)) {
      return match(tokenKinds...);
    } else {
      return false;
    }
  }

  void report(SMLoc loc, SourceMgr::DiagKind kind, StringRef msg,
              raw_ostream &os = llvm::errs()) {
    SrcMgr.PrintMessage(os, loc, kind, msg);
    if (kind == SourceMgr::DK_Error)
      errorNum++;
  }

  unsigned errorNum;
  const ArrayRef<uptr<Token>> &tokens;
  std::size_t cursor;
};

} // namespace lox

#endif
