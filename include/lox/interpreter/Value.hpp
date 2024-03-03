#ifndef __LOX_VALUE_HPP__
#define __LOX_VALUE_HPP__

#include "utils/TypeUtils.hpp"
#include "llvm/ADT/StringRef.h"
#include <string>

namespace lox {
using namespace llvm;

enum ValueKind : unsigned {
  Number,
  String,
  Bool,
  Nil,
};

class Value {
public:
  explicit Value(ValueKind kind) : kind(kind) {}
  virtual ~Value() = default;

  [[nodiscard]] auto getKind() const { return kind; }
  [[nodiscard]] virtual std::string str() const = 0;
  [[nodiscard]] virtual bool truthy() const = 0;
  [[nodiscard]] virtual bool operator==(const Value &other) const = 0;

private:
  ValueKind kind;
};

class NumberValue : public Value {
public:
  explicit NumberValue(const long double value) : Value(Number), value(value) {}

  [[nodiscard]] std::string str() const override;
  [[nodiscard]] bool truthy() const override;
  [[nodiscard]] bool operator==(const Value &other) const override;

  static bool classof(const Value *value);

  [[nodiscard]] auto getValue() const { return value; }

private:
  long double value;
};

class StringValue : public Value {
public:
  explicit StringValue(const StringRef str) : Value(String), value(str.str()) {}

  [[nodiscard]] std::string str() const override;
  [[nodiscard]] bool truthy() const override;
  [[nodiscard]] bool operator==(const Value &other) const override;

  static bool classof(const Value *value);

  [[nodiscard]] auto getValue() const { return value; }

private:
  std::string value;
};

class BoolValue : public Value {
public:
  explicit BoolValue(const bool value) : Value(Bool), value(value) {}
  [[nodiscard]] std::string str() const override;
  [[nodiscard]] bool truthy() const override;
  [[nodiscard]] bool operator==(const Value &other) const override;

  static bool classof(const Value *value);

private:
  bool value;
};

class NilValue : public Value {
public:
  NilValue() : Value(Nil) {}
  [[nodiscard]] std::string str() const override;
  [[nodiscard]] bool truthy() const override;
  [[nodiscard]] bool operator==(const Value &other) const override;

  static bool classof(const Value *value);
};

} // namespace lox

#endif // __LOX_VALUE_HPP__