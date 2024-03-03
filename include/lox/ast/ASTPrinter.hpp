#ifndef __AST_VISITOR_HPP__
#define __AST_VISITOR_HPP__

#include "lox/ast/AST.hpp"
#include "llvm/Support/raw_ostream.h"
#include <variant>

namespace lox {
using namespace llvm;

template <typename ConcreteType>
struct PrinterBase {

  ConcreteType &get() { return static_cast<ConcreteType &>(*this); }
  const ConcreteType &get() const {
    return static_cast<const ConcreteType &>(*this);
  }

  void clear() { get().clear(); }

  std::string getResult() const { return get().getResult(); }
};

class ExprPrinter;
class StmtPrinter;

struct ASTPrinter : PrinterBase<ASTPrinter> {
  ASTPrinter(ExprPrinter &exprPrinter, StmtPrinter &stmtPrinter)
      : exprPrinter(exprPrinter), stmtPrinter(stmtPrinter), ss(result) {}

  void operator()(const Expr &expr);
  void operator()(const Stmt &stmt);

  void clear();
  std::string getResult() const { return result; }

private:
  ExprPrinter &exprPrinter;
  StmtPrinter &stmtPrinter;
  std::string result;
  raw_string_ostream ss;
};

struct ExprPrinter : PrinterBase<ExprPrinter> {
  ExprPrinter() : ss(result) {}

  void operator()(const BinaryE &binaryE);
  void operator()(const UnaryE &unaryE);
  void operator()(const GroupingE &groupingE);
  void operator()(const LiteralE &literalE);

  void clear();
  std::string getResult() const { return result; }

private:
  std::string result;
  raw_string_ostream ss;
};

struct StmtPrinter : PrinterBase<StmtPrinter> {
  StmtPrinter() : ss(result) {}

  void operator()(const ExprStmt &exprStmt);
  void operator()(const ClassStmt &classStmt);

  void clear();
  std::string getResult() const { return result; }

private:
  std::string result;
  raw_string_ostream ss;
};

} // namespace lox

#endif // __AST_VISITOR_HPP__
