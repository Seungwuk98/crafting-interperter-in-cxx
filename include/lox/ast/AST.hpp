#ifndef __LOX_AST_HPP__
#define __LOX_AST_HPP__

#include "lox/parser/Token.hpp"
#include "utils/TypeUtils.hpp"
#include "llvm/ADT/SmallVector.h"
#include <variant>

namespace lox {

///==------------------------------- Expr ---------------------------------==///

class BinaryE;
class UnaryE;
class GroupingE;
class LiteralE;
class VarE;
class AssignE;

using Expr = std::variant<BinaryE, UnaryE, GroupingE, LiteralE, VarE, AssignE>;

template <typename ConcreteType>
class ASTBase {
public:
  ConcreteType &get() { return static_cast<ConcreteType &>(*this); }
  [[nodiscard]] const ConcreteType &get() const {
    return static_cast<const ConcreteType &>(*this);
  }

  [[nodiscard]] SMLoc getLoc() const { return loc; }

protected:
  explicit ASTBase(const SMLoc loc) : loc(loc) {}
  ~ASTBase() = default;

private:
  SMLoc loc;
};

class BinaryE : public ASTBase<BinaryE> {
public:
  BinaryE(const SMLoc loc, uptr<Expr> lhs, uptr<Expr> rhs, Token *opKind)
      : ASTBase(loc), lhs(std::move(lhs)), rhs(std::move(rhs)), opKind(opKind) {
  }
  [[nodiscard]] Expr *getLhs() const { return lhs.get(); }
  [[nodiscard]] Expr *getRhs() const { return rhs.get(); }
  [[nodiscard]] Token *getOpKind() const { return opKind; }

private:
  uptr<Expr> lhs;
  uptr<Expr> rhs;
  Token *opKind;
};

class UnaryE : public ASTBase<UnaryE> {
public:
  UnaryE(const SMLoc loc, uptr<Expr> expr, Token *opKind)
      : ASTBase(loc), expr(std::move(expr)), opKind(opKind) {}
  [[nodiscard]] Expr *getExpr() const { return expr.get(); }
  [[nodiscard]] Token *getOpKind() const { return opKind; }

private:
  uptr<Expr> expr;
  Token *opKind;
};

class GroupingE : public ASTBase<GroupingE> {
public:
  GroupingE(const SMLoc loc, uptr<Expr> expr)
      : ASTBase(loc), expr(std::move(expr)) {}
  [[nodiscard]] Expr *getExpr() const { return expr.get(); }

private:
  uptr<Expr> expr;
};

class LiteralE : public ASTBase<LiteralE> {
public:
  LiteralE(const SMLoc loc, Token *value) : ASTBase(loc), value(value) {}
  [[nodiscard]] Token *getValue() const { return value; }

private:
  Token *value;
};

class VarE : public ASTBase<VarE> {
public:
  VarE(const SMLoc loc, Token *symbol) : ASTBase(loc), symbol(symbol) {}
  [[nodiscard]] Token *getSymbol() const { return symbol; }

private:
  Token *symbol;
};

class AssignE : public ASTBase<AssignE> {
public:
  AssignE(const SMLoc loc, Token *symbol, uptr<Expr> value)
      : ASTBase(loc), symbol(symbol), value(std::move(value)) {}

  [[nodiscard]] Token *getSymbol() const { return symbol; }
  [[nodiscard]] Expr *getValue() const { return value.get(); }

private:
  Token *symbol;
  uptr<Expr> value;
};

inline SMLoc getLoc(const Expr &expr) {
  if (auto *unaryE = std::get_if<UnaryE>(&expr))
    return unaryE->getLoc();
  if (auto *binaryE = std::get_if<BinaryE>(&expr))
    return binaryE->getLoc();
  if (auto *groupingE = std::get_if<GroupingE>(&expr))
    return groupingE->getLoc();
  if (auto *literalE = std::get_if<LiteralE>(&expr))
    return literalE->getLoc();
  if (auto *varE = std::get_if<VarE>(&expr))
    return varE->getLoc();
  return {};
}

///==----------------------------- Statement ------------------------------==///

class ExprStmt;
class PrintStmt;
class VarStmt;

using Stmt = std::variant<ExprStmt, PrintStmt, VarStmt>;
using Program = SmallVector<uptr<Stmt>, 16u>;

class ExprStmt : ASTBase<ExprStmt> {
public:
  ExprStmt(const SMLoc loc, uptr<Expr> expr)
      : ASTBase(loc), expr(std::move(expr)) {}
  [[nodiscard]] Expr *getExpr() const { return expr.get(); }

private:
  uptr<Expr> expr;
};

class PrintStmt : ASTBase<PrintStmt> {
public:
  PrintStmt(const SMLoc loc, uptr<Expr> expr)
      : ASTBase(loc), expr(std::move(expr)) {}
  [[nodiscard]] Expr *getExpr() const { return expr.get(); }

private:
  uptr<Expr> expr;
};

class VarStmt : ASTBase<VarStmt> {
public:
  VarStmt(const SMLoc loc, Token *symbol, uptr<Expr> init)
      : ASTBase(loc), symbol(symbol), init(std::move(init)) {}

  [[nodiscard]] Token *getSymbol() const { return symbol; }
  [[nodiscard]] Expr *getInit() const { return init.get(); }

private:
  Token *symbol;
  uptr<Expr> init;
};

using AST = std::variant<Expr, Stmt>;

} // namespace lox

#endif // __LOX_AST_HPP__
