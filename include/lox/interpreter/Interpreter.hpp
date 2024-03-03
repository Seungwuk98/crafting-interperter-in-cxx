#ifndef __LOX_INTERPRETER_HPP__
#define __LOX_INTERPRETER_HPP__

#include "lox/ast/AST.hpp"
#include "lox/interpreter/Value.hpp"
#include "utils/TypeUtils.hpp"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

namespace lox {

struct ExprInterpreter;

struct StmtInterpreter {
  explicit StmtInterpreter(SourceMgr &srcMgr, ExprInterpreter &exprInterpreter,
                           raw_ostream &os = llvm::errs())
      : SrcMgr(srcMgr), ExprEvaluator(exprInterpreter), os(os), error(0u) {}

  SourceMgr &SrcMgr;
  ExprInterpreter &ExprEvaluator;

  opt<uptr<Value>> Evaluate(const Expr &expr) const;

  bool operator()(const ExprStmt &exprStmt);
  bool operator()(const PrintStmt &printStmt);

  [[nodiscard]] std::size_t getError() const;

private:
  /// redirect of print
  raw_ostream &os;
  std::size_t error;
};

struct ExprInterpreter {
  explicit ExprInterpreter(SourceMgr &srcMgr) : SrcMgr(srcMgr), error(0u) {}

  opt<uptr<Value>> operator()(const BinaryE &binaryE);
  opt<uptr<Value>> operator()(const UnaryE &unaryE);
  opt<uptr<Value>> operator()(const GroupingE &groupingE);
  opt<uptr<Value>> operator()(const LiteralE &literalE);

  SourceMgr &SrcMgr;

  [[nodiscard]] std::size_t getError() const { return error; }

private:
  void report(SMLoc loc, SourceMgr::DiagKind kind, StringRef msg,
              raw_ostream &os = llvm::errs());

  std::size_t error;
};

} // namespace lox

#endif /// __LOX_INTERPRETER_HPP__
