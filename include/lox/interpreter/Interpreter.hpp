#ifndef LOW_INTERPRETER_HPP
#define LOW_INTERPRETER_HPP

#include "lox/ast/AST.hpp"
#include "lox/interpreter/Value.hpp"
#include "utils/TypeUtils.hpp"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/ScopedHashTable.h"

namespace lox {

struct ExprInterpreter;

using LoxValueEnv = ScopedHashTable<StringRef, sptr<Value>>;
using LoxValueScope = LoxValueEnv::ScopeTy;

struct StmtInterpreter {
  explicit StmtInterpreter(SourceMgr &srcMgr, ExprInterpreter &exprInterpreter,
                           LoxValueEnv &env, raw_ostream &os = llvm::errs())
      : SrcMgr(srcMgr), ExprEvaluator(exprInterpreter), env(env), os(os),
        error(0u) {}

  SourceMgr &SrcMgr;
  ExprInterpreter &ExprEvaluator;

  [[nodiscard]] sptr<Value> Evaluate(const Expr &expr) const;

  bool operator()(const ExprStmt &exprStmt);
  bool operator()(const PrintStmt &printStmt);
  bool operator()(const VarStmt &varStmt);

  [[nodiscard]] std::size_t getError() const;

private:
  LoxValueEnv &env;
  /// redirect of print
  raw_ostream &os;
  std::size_t error;
};

struct ExprInterpreter {
  explicit ExprInterpreter(SourceMgr &srcMgr, LoxValueEnv &env)
      : SrcMgr(srcMgr), error(0u), env(env) {}

  sptr<Value> operator()(const BinaryE &binaryE);
  sptr<Value> operator()(const UnaryE &unaryE);
  sptr<Value> operator()(const GroupingE &groupingE);
  sptr<Value> operator()(const LiteralE &literalE);
  sptr<Value> operator()(const VarE &varE);
  sptr<Value> operator()(const AssignE &assignE);

  SourceMgr &SrcMgr;

  [[nodiscard]] std::size_t getError() const { return error; }

private:
  void report(SMLoc loc, SourceMgr::DiagKind kind, StringRef msg,
              raw_ostream &os = llvm::errs());

  std::size_t error;
  LoxValueEnv &env;
};

} // namespace lox

#endif /// LOW_INTERPRETER_HPP
