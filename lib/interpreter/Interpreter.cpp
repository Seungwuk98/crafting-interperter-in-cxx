#include "lox/interpreter/Interpreter.hpp"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"

#define CheckNumberOperand(loc, value)                                         \
  {}

namespace lox {

opt<uptr<Value>> StmtInterpreter::Evaluate(const Expr &expr) const {
  return std::visit(ExprEvaluator, expr);
}
bool StmtInterpreter::operator()(const ExprStmt &exprStmt) {
  if (Evaluate(*exprStmt.getExpr()))
    return false;
  return true;
}

bool StmtInterpreter::operator()(const PrintStmt &printStmt) {
  const auto exprResultOpt = Evaluate(*printStmt.getExpr());
  if (!exprResultOpt)
    return false;
  os << exprResultOpt.value()->str() << '\n';
  return true;
}

std::size_t StmtInterpreter::getError() const {
  return error + ExprEvaluator.getError();
}

opt<uptr<Value>> ExprInterpreter::operator()(const BinaryE &binaryE) {
  const auto lhsVOpt = std::visit(*this, *binaryE.getLhs());
  if (!lhsVOpt)
    return {};
  const auto rhsVOpt = std::visit(*this, *binaryE.getRhs());
  if (!rhsVOpt)
    return {};

  const auto *lhsV = lhsVOpt->get();
  const auto *rhsV = rhsVOpt->get();

  switch (const auto *opKind = binaryE.getOpKind(); opKind->Kind) {
  case Tok_minus:
    if (NumberValue::classof(lhsV) && NumberValue::classof(rhsV)) {
      return mkptr<NumberValue>(llvm::cast<NumberValue>(lhsV)->getValue() -
                                llvm::cast<NumberValue>(rhsV)->getValue());
    } else {
      report(binaryE.getLoc(), SourceMgr::DK_Error,
             "Operands must be a number");
      return {};
    }
  case Tok_plus:
    if (NumberValue::classof(lhsV) && NumberValue::classof(rhsV)) {
      return mkptr<NumberValue>(llvm::cast<NumberValue>(lhsV)->getValue() +
                                llvm::cast<NumberValue>(rhsV)->getValue());
    } else if (StringValue::classof(lhsV) && StringValue::classof(rhsV)) {
      return mkptr<StringValue>(llvm::cast<StringValue>(lhsV)->getValue() +
                                llvm::cast<StringValue>(rhsV)->getValue());
    } else {
      report(binaryE.getLoc(), SourceMgr::DK_Error,
             "Operands must be a number or string");
      return {};
    }
  case Tok_slash:
    if (NumberValue::classof(lhsV) && NumberValue::classof(rhsV)) {
      return mkptr<NumberValue>(llvm::cast<NumberValue>(lhsV)->getValue() /
                                llvm::cast<NumberValue>(rhsV)->getValue());
    } else {
      report(binaryE.getLoc(), SourceMgr::DK_Error,
             "Operands must be a number");
      return {};
    }
  case Tok_star:
    if (NumberValue::classof(lhsV) && NumberValue::classof(rhsV)) {
      return mkptr<NumberValue>(llvm::cast<NumberValue>(lhsV)->getValue() *
                                llvm::cast<NumberValue>(rhsV)->getValue());
    } else {
      report(binaryE.getLoc(), SourceMgr::DK_Error,
             "Operands must be a number");
      return {};
    }
  case Tok_ge:
    if (NumberValue::classof(lhsV) && NumberValue::classof(rhsV)) {
      return mkptr<BoolValue>(llvm::cast<NumberValue>(lhsV)->getValue() >=
                              llvm::cast<NumberValue>(rhsV)->getValue());
    } else {
      report(binaryE.getLoc(), SourceMgr::DK_Error,
             "Operands must be a number");
      return {};
    }
  case Tok_gt:
    if (NumberValue::classof(lhsV) && NumberValue::classof(rhsV)) {
      return mkptr<BoolValue>(llvm::cast<NumberValue>(lhsV)->getValue() >
                              llvm::cast<NumberValue>(rhsV)->getValue());
    } else {
      report(binaryE.getLoc(), SourceMgr::DK_Error,
             "Operands must be a number");
      return {};
    }
  case Tok_le:
    if (NumberValue::classof(lhsV) && NumberValue::classof(rhsV)) {
      return mkptr<BoolValue>(llvm::cast<NumberValue>(lhsV)->getValue() <=
                              llvm::cast<NumberValue>(rhsV)->getValue());
    } else {
      report(binaryE.getLoc(), SourceMgr::DK_Error,
             "Operands must be a number");
      return {};
    }
  case Tok_lt:
    if (NumberValue::classof(lhsV) && NumberValue::classof(rhsV)) {
      return mkptr<BoolValue>(llvm::cast<NumberValue>(lhsV)->getValue() <
                              llvm::cast<NumberValue>(rhsV)->getValue());
    } else {
      report(binaryE.getLoc(), SourceMgr::DK_Error,
             "Operands must be a number");
      return {};
    }
  case Tok_equal_equal:
    return mkptr<BoolValue>(*lhsV == *rhsV);
  case Tok_bang_equal:
    return mkptr<BoolValue>(!(*lhsV == *rhsV));

  default:
    llvm_unreachable("runtime error");
  }
}

opt<uptr<Value>> ExprInterpreter::operator()(const UnaryE &unaryE) {
  const auto targetVOpt = std::visit(*this, *unaryE.getExpr());
  if (!targetVOpt)
    return {};
  const auto *targetV = targetVOpt->get();

  switch (unaryE.getOpKind()->Kind) {
  case Tok_minus: {
    if (const auto *numberV = llvm::dyn_cast<NumberValue>(targetV))
      return mkptr<NumberValue>(-numberV->getValue());
    else {
      report(unaryE.getLoc(), SourceMgr::DK_Error, "Operand must be a number");
      return {};
    }
  }
  case Tok_bang: {
    const auto truthy = targetV->truthy();
    return mkptr<BoolValue>(!truthy);
  }
  default:
    llvm_unreachable("all types of unary expression was handled");
  }
}

opt<uptr<Value>> ExprInterpreter::operator()(const GroupingE &groupingE) {
  return std::visit(*this, *groupingE.getExpr());
}

opt<uptr<Value>> ExprInterpreter::operator()(const LiteralE &literalE) {
  switch (const auto *tok = literalE.getValue(); tok->Kind) {
  case Tok_number:
    return mkptr<NumberValue>(std::stold(tok->Symbol.str()));
  case Tok_string:
    return mkptr<StringValue>(tok->Symbol.str());
  case Tok_true:
    return mkptr<BoolValue>(true);
  case Tok_false:
    return mkptr<BoolValue>(false);
  case Tok_nil:
    return mkptr<NilValue>();
  default:
    llvm_unreachable("All of literal is handled");
  }
}

void ExprInterpreter::report(const SMLoc loc, const SourceMgr::DiagKind kind,
                             const StringRef msg, raw_ostream &os) {
  SrcMgr.PrintMessage(os, loc, kind, msg);
  if (kind == SourceMgr::DK_Error)
    error++;
}

} // namespace lox