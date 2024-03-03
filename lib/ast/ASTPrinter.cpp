#include "lox/ast/ASTPrinter.hpp"
#include <llvm/Support/ErrorHandling.h>

namespace lox {

void ExprPrinter::operator()(const lox::UnaryE &unaryE) {
  ss << unaryE.getOpKind()->Symbol;
  std::visit(*this, *unaryE.getExpr());
}

void ExprPrinter::operator()(const lox::BinaryE &binaryE) {
  ss << '(';
  std::visit(*this, *binaryE.getLhs());
  ss << ' ' << binaryE.getOpKind()->Symbol << ' ';
  std::visit(*this, *binaryE.getRhs());
  ss << ')';
}

void ExprPrinter::operator()(const lox::LiteralE &literalE) {
  auto opTok = literalE.getValue();
  if (opTok->Kind == TokenKind::Tok_string)
    ss << '"' << opTok->Symbol << '"';
  else if (opTok->Kind == TokenKind::Tok_number)
    ss << opTok->Symbol;
  else
    llvm_unreachable("In lox, there are only string and number literal");
}

void StmtPrinter::operator()(const ExprStmt &exprStmt) {
  exprPrint(*exprStmt.getExpr());
  ss << ";\n";
}

void StmtPrinter::operator()(const PrintStmt &printStmt) {
  ss << "print ";
  exprPrint(*printStmt.getExpr());
  ss << ";\n";
}

void StmtPrinter::exprPrint(const Expr &expr) {
  std::visit(ExprFormatter, expr);
}

void ExprPrinter::operator()(const lox::GroupingE &groupingE) {
  ss << '(';
  std::visit(*this, *groupingE.getExpr());
  ss << ')';
}

} // namespace lox
