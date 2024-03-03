#include "lox/interpreter/Value.hpp"
#include "llvm/Support/Casting.h"

namespace lox {

std::string NumberValue::str() const { return std::to_string(value); }
bool NumberValue::truthy() const { return false; }
bool NumberValue::classof(const Value *value) {
  return value->getKind() == Number;
}
bool NumberValue::operator==(const Value &other) const {
  if (const auto *number = dyn_cast<NumberValue>(&other))
    return value == number->value;
  return false;
}

std::string StringValue::str() const { return value; }
bool StringValue::truthy() const { return false; }
bool StringValue::classof(const Value *value) {
  return value->getKind() == String;
}
bool StringValue::operator==(const Value &other) const {
  if (const auto *str = dyn_cast<StringValue>(&other))
    return value == str->value;
  return false;
}

std::string BoolValue::str() const { return value ? "true" : "false"; }
bool BoolValue::truthy() const { return value; }
bool BoolValue::classof(const Value *value) { return value->getKind() == Bool; }
bool BoolValue::operator==(const Value &other) const {
  if (const auto *boolean = dyn_cast<BoolValue>(&other))
    return value == boolean->value;
  return false;
}

std::string NilValue::str() const { return "nil"; }
bool NilValue::truthy() const { return false; }
bool NilValue::classof(const Value *value) { return value->getKind() == Nil; }
bool NilValue::operator==(const Value &other) const {
  if (const auto *nil = dyn_cast<NilValue>(&other))
    return true;
  return false;
}
} // namespace lox