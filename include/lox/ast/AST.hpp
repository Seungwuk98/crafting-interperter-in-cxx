#ifndef __LOX_AST_HPP__
#define __LOX_AST_HPP__

#include "lox/parser/Token.hpp"
#include "utils/TypeUtils.hpp"
#include <variant>

namespace lox {

///==------------------------------- Expr ---------------------------------==///

class BinaryE;
class UnaryE;
class GroupingE;
class LiteralE;

using Expr = std::variant<BinaryE, UnaryE, GroupingE, LiteralE>;

class BinaryE {
public:
  BinaryE(uptr<Expr> lhs, uptr<Expr> rhs, Token *opKind)
      : lhs(std::move(lhs)), rhs(std::move(rhs)), opKind(opKind) {}
  [[nodiscard]] Expr *getLhs() const { return lhs.get(); }
  [[nodiscard]] Expr *getRhs() const { return rhs.get(); }
  [[nodiscard]] Token *getOpKind() const { return opKind; }

private:
  uptr<Expr> lhs;
  uptr<Expr> rhs;
  Token *opKind;
};

class UnaryE {
public:
  UnaryE(uptr<Expr> expr, Token *opKind)
      : expr(std::move(expr)), opKind(opKind) {}
  [[nodiscard]] Expr *getExpr() const { return expr.get(); }
  [[nodiscard]] Token *getOpKind() const { return opKind; }

private:
  uptr<Expr> expr;
  Token *opKind;
};

class GroupingE {
public:
  explicit GroupingE(uptr<Expr> expr) : expr(std::move(expr)) {}
  [[nodiscard]] Expr *getExpr() const { return expr.get(); }

private:
  uptr<Expr> expr;
};

class LiteralE {
public:
  explicit LiteralE(Token *value) : value(value) {}
  [[nodiscard]] Token *getValue() const { return value; }

private:
  Token *value;
};

///==----------------------------- Statement ------------------------------==///

class ExprStmt;
class ClassStmt;

using Stmt = std::variant<ExprStmt, ClassStmt>;

using AST = std::variant<Expr, Stmt>;

} // namespace lox

#endif // __LOX_AST_HPP__
