#include "lox/interpreter/Interpreter.hpp"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"

#include <llvm/Support/FormatVariadic.h>

namespace lox {

sptr<Value> StmtInterpreter::Evaluate(const Expr &expr) const {
  return std::visit(ExprEvaluator, expr);
}

bool StmtInterpreter::operator()(const ExprStmt &exprStmt) {
  if (Evaluate(*exprStmt.getExpr()))
    return false;
  return true;
}

bool StmtInterpreter::operator()(const PrintStmt &printStmt) {
  const auto exprValuePtr = Evaluate(*printStmt.getExpr());
  if (!exprValuePtr)
    return false;
  os << exprValuePtr->str() << '\n';
  return true;
}

bool StmtInterpreter::operator()(const VarStmt &varStmt) {
  sptr<Value> init;
  if (varStmt.getInit()) {
    init = std::visit(ExprEvaluator, *varStmt.getInit());
    if (!init)
      return false;
  }

  env.insert(varStmt.getSymbol()->Symbol, init);
  return true;
}

std::size_t StmtInterpreter::getError() const {
  return error + ExprEvaluator.getError();
}

sptr<Value> ExprInterpreter::operator()(const BinaryE &binaryE) {
  const auto lhsVPtr = std::visit(*this, *binaryE.getLhs());
  const auto rhsVPtr = std::visit(*this, *binaryE.getRhs());
  if (!lhsVPtr || !rhsVPtr)
    return nullptr;

  const auto lhsV = lhsVPtr.get();
  const auto rhsV = rhsVPtr.get();

  switch (const auto *opKind = binaryE.getOpKind(); opKind->Kind) {
  case Tok_minus:
    if (NumberValue::classof(lhsV) && NumberValue::classof(rhsV)) {
      return mkuptr<NumberValue>(llvm::cast<NumberValue>(lhsV)->getValue() -
                                 llvm::cast<NumberValue>(rhsV)->getValue());
    } else {
      report(binaryE.getLoc(), SourceMgr::DK_Error,
             "Operands must be a number");
      return {};
    }
  case Tok_plus:
    if (NumberValue::classof(lhsV) && NumberValue::classof(rhsV)) {
      return mkuptr<NumberValue>(llvm::cast<NumberValue>(lhsV)->getValue() +
                                 llvm::cast<NumberValue>(rhsV)->getValue());
    } else if (StringValue::classof(lhsV) && StringValue::classof(rhsV)) {
      return mkuptr<StringValue>(llvm::cast<StringValue>(lhsV)->getValue() +
                                 llvm::cast<StringValue>(rhsV)->getValue());
    } else {
      report(binaryE.getLoc(), SourceMgr::DK_Error,
             "Operands must be a number or string");
      return {};
    }
  case Tok_slash:
    if (NumberValue::classof(lhsV) && NumberValue::classof(rhsV)) {
      return mkuptr<NumberValue>(llvm::cast<NumberValue>(lhsV)->getValue() /
                                 llvm::cast<NumberValue>(rhsV)->getValue());
    } else {
      report(binaryE.getLoc(), SourceMgr::DK_Error,
             "Operands must be a number");
      return {};
    }
  case Tok_star:
    if (NumberValue::classof(lhsV) && NumberValue::classof(rhsV)) {
      return mkuptr<NumberValue>(llvm::cast<NumberValue>(lhsV)->getValue() *
                                 llvm::cast<NumberValue>(rhsV)->getValue());
    } else {
      report(binaryE.getLoc(), SourceMgr::DK_Error,
             "Operands must be a number");
      return {};
    }
  case Tok_ge:
    if (NumberValue::classof(lhsV) && NumberValue::classof(rhsV)) {
      return mkuptr<BoolValue>(llvm::cast<NumberValue>(lhsV)->getValue() >=
                               llvm::cast<NumberValue>(rhsV)->getValue());
    } else {
      report(binaryE.getLoc(), SourceMgr::DK_Error,
             "Operands must be a number");
      return {};
    }
  case Tok_gt:
    if (NumberValue::classof(lhsV) && NumberValue::classof(rhsV)) {
      return mkuptr<BoolValue>(llvm::cast<NumberValue>(lhsV)->getValue() >
                               llvm::cast<NumberValue>(rhsV)->getValue());
    } else {
      report(binaryE.getLoc(), SourceMgr::DK_Error,
             "Operands must be a number");
      return {};
    }
  case Tok_le:
    if (NumberValue::classof(lhsV) && NumberValue::classof(rhsV)) {
      return mkuptr<BoolValue>(llvm::cast<NumberValue>(lhsV)->getValue() <=
                               llvm::cast<NumberValue>(rhsV)->getValue());
    } else {
      report(binaryE.getLoc(), SourceMgr::DK_Error,
             "Operands must be a number");
      return {};
    }
  case Tok_lt:
    if (NumberValue::classof(lhsV) && NumberValue::classof(rhsV)) {
      return mkuptr<BoolValue>(llvm::cast<NumberValue>(lhsV)->getValue() <
                               llvm::cast<NumberValue>(rhsV)->getValue());
    } else {
      report(binaryE.getLoc(), SourceMgr::DK_Error,
             "Operands must be a number");
      return {};
    }
  case Tok_equal_equal:
    return mkuptr<BoolValue>(*lhsV == *rhsV);
  case Tok_bang_equal:
    return mkuptr<BoolValue>(!(*lhsV == *rhsV));

  default:
    llvm_unreachable("runtime error");
  }
}

sptr<Value> ExprInterpreter::operator()(const UnaryE &unaryE) {
  const auto targetVPtr = std::visit(*this, *unaryE.getExpr());
  if (!targetVPtr)
    return nullptr;
  const auto *targetV = targetVPtr.get();

  switch (unaryE.getOpKind()->Kind) {
  case Tok_minus: {
    if (const auto *numberV = llvm::dyn_cast<NumberValue>(targetV))
      return mkuptr<NumberValue>(-numberV->getValue());
    else {
      report(unaryE.getLoc(), SourceMgr::DK_Error, "Operand must be a number");
      return {};
    }
  }
  case Tok_bang: {
    const auto truthy = targetV->truthy();
    return mkuptr<BoolValue>(!truthy);
  }
  default:
    llvm_unreachable("all types of unary expression was handled");
  }
}

sptr<Value> ExprInterpreter::operator()(const GroupingE &groupingE) {
  return std::visit(*this, *groupingE.getExpr());
}

sptr<Value> ExprInterpreter::operator()(const LiteralE &literalE) {
  switch (const auto *tok = literalE.getValue(); tok->Kind) {
  case Tok_number:
    return mkuptr<NumberValue>(std::stold(tok->Symbol.str()));
  case Tok_string:
    return mkuptr<StringValue>(tok->Symbol.str());
  case Tok_true:
    return mkuptr<BoolValue>(true);
  case Tok_false:
    return mkuptr<BoolValue>(false);
  case Tok_nil:
    return mkuptr<NilValue>();
  default:
    llvm::errs() << getTokenName(tok->Kind) << '\n';
    llvm_unreachable("All of literal types are handled");
  }
}

sptr<Value> ExprInterpreter::operator()(const VarE &varE) {
  auto symbol = varE.getSymbol()->Symbol;
  if (!env.count(symbol)) {
    report(varE.getLoc(), SourceMgr::DK_Error,
           formatv("Undefined variable : {0}", symbol).str());
    return nullptr;
  }
  return env.lookup(symbol);
}

sptr<Value> ExprInterpreter::operator()(const AssignE &assignE) {
  auto symbol = assignE.getSymbol();
  if (!env.count(symbol->Symbol)) {
    report(symbol->Loc, SourceMgr::DK_Error,
           formatv("Undefined variable : {0}", symbol).str());
    return nullptr;
  }
  const auto value = std::visit(*this, *assignE.getValue());
  if (!value)
    return nullptr;
  env.insert(symbol->Symbol, value);
  return value;
}

void ExprInterpreter::report(const SMLoc loc, const SourceMgr::DiagKind kind,
                             const StringRef msg, raw_ostream &os) {
  SrcMgr.PrintMessage(os, loc, kind, msg);
  if (kind == SourceMgr::DK_Error)
    error++;
}

} // namespace lox