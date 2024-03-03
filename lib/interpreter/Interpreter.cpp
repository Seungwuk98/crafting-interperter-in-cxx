#include "lox/interpreter/Interpreter.hpp"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"

namespace lox {

uptr<Value> ExprInterpreter::operator()(const BinaryE &binaryE) {
  const auto lhsV = std::visit(*this, *binaryE.getLhs());
  const auto rhsV = std::visit(*this, *binaryE.getRhs());
  const auto *opKind = binaryE.getOpKind();

  switch (opKind->Kind) {
  case Tok_minus:
    if (NumberValue::classof(lhsV.get()) && NumberValue::classof(rhsV.get())) {
      return mkptr<NumberValue>(
          llvm::cast<NumberValue>(lhsV.get())->getValue() -
          llvm::cast<NumberValue>(rhsV.get())->getValue());
    }
  case Tok_plus:
    if (NumberValue::classof(lhsV.get()) && NumberValue::classof(rhsV.get())) {
      return mkptr<NumberValue>(
          llvm::cast<NumberValue>(lhsV.get())->getValue() +
          llvm::cast<NumberValue>(rhsV.get())->getValue());
    } else if (StringValue::classof(lhsV.get()) &&
               StringValue::classof(rhsV.get())) {
      return mkptr<StringValue>(
          llvm::cast<StringValue>(lhsV.get())->getValue() +
          llvm::cast<StringValue>(rhsV.get())->getValue());
    }
  case Tok_slash:
    if (NumberValue::classof(lhsV.get()) && NumberValue::classof(rhsV.get())) {
      return mkptr<NumberValue>(
          llvm::cast<NumberValue>(lhsV.get())->getValue() /
          llvm::cast<NumberValue>(rhsV.get())->getValue());
    }
  case Tok_star:
    if (NumberValue::classof(lhsV.get()) && NumberValue::classof(rhsV.get())) {
      return mkptr<NumberValue>(
          llvm::cast<NumberValue>(lhsV.get())->getValue() *
          llvm::cast<NumberValue>(rhsV.get())->getValue());
    }
  case Tok_ge:
    if (NumberValue::classof(lhsV.get()) && NumberValue::classof(rhsV.get())) {
      return mkptr<BoolValue>(llvm::cast<NumberValue>(lhsV.get())->getValue() >=
                              llvm::cast<NumberValue>(rhsV.get())->getValue());
    }
  case Tok_gt:
    if (NumberValue::classof(lhsV.get()) && NumberValue::classof(rhsV.get())) {
      return mkptr<BoolValue>(llvm::cast<NumberValue>(lhsV.get())->getValue() >
                              llvm::cast<NumberValue>(rhsV.get())->getValue());
    }
  case Tok_le:
    if (NumberValue::classof(lhsV.get()) && NumberValue::classof(rhsV.get())) {
      return mkptr<BoolValue>(llvm::cast<NumberValue>(lhsV.get())->getValue() <=
                              llvm::cast<NumberValue>(rhsV.get())->getValue());
    }
  case Tok_lt:
    if (NumberValue::classof(lhsV.get()) && NumberValue::classof(rhsV.get())) {
      return mkptr<BoolValue>(llvm::cast<NumberValue>(lhsV.get())->getValue() <
                              llvm::cast<NumberValue>(rhsV.get())->getValue());
    }
  case Tok_equal_equal:
    return mkptr<BoolValue>(*lhsV == *rhsV);
  case Tok_bang_equal:
    return mkptr<BoolValue>(!(*lhsV == *rhsV));

  default:
    llvm_unreachable("runtime error");
  }
}

uptr<Value> ExprInterpreter::operator()(const UnaryE &unaryE) {
  const auto targetV = std::visit(*this, *unaryE.getExpr());

  switch (unaryE.getOpKind()->Kind) {
  case Tok_minus: {
    if (const auto *numberV = llvm::dyn_cast<NumberValue>(targetV.get()))
      return mkptr<NumberValue>(-numberV->getValue());
    else
      llvm_unreachable("runtime error");
  }
  case Tok_bang: {
    const auto truthy = targetV->truthy();
    return mkptr<BoolValue>(!truthy);
  }
  default:
    llvm_unreachable("All of unary operation is implemented");
  }
}

uptr<Value> ExprInterpreter::operator()(const GroupingE &groupingE) {
  return std::visit(*this, *groupingE.getExpr());
}

uptr<Value> ExprInterpreter::operator()(const LiteralE &literalE) {
  const auto *tok = literalE.getValue();
  switch (tok->Kind) {
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

} // namespace lox