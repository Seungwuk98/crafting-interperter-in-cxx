#ifndef __INTERPRETER_TEST_UTILS_HPP__
#define __INTERPRETER_TEST_UTILS_HPP__

#include "doctest/doctest.h"
#include "lox/interpreter/Interpreter.hpp"
#include "lox/parser/Lexer.hpp"
#include "lox/parser/Parser.hpp"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/FormatVariadic.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"

namespace lox {

inline bool runInterpreterTest(StringRef code, const Value &value) {
  auto bufferPtr = MemoryBuffer::getMemBuffer(code, "source");
  const auto &buffer = *bufferPtr;
  SourceMgr srcMgr;
  const auto id = srcMgr.AddNewSourceBuffer(std::move(bufferPtr), {});
  Lexer lexer(srcMgr, buffer.getBuffer());
  if (const auto success = lexer.Lex(); !success)
    return false;

  Parser parser(lexer.getTokens(), srcMgr);
  auto expr = parser.Expression();
  if (!expr)
    return false;

  LoxValueEnv Env;
  ExprInterpreter exprInterpreter(srcMgr, Env);
  auto result = std::visit(exprInterpreter, *expr);
  if (exprInterpreter.getError())
    return false;

  if (!(*result == value)) {
    FAIL(llvm::formatv("expected : {0}, got : {1}", value.str(), result->str())
             .str());
    return false;
  }

  return true;
}

#define INTERPRETER_TEST(test, code, expect)                                   \
  SUBCASE(test) CHECK(::lox::runInterpreterTest(code, expect))

inline bool runPrintInterpretTest(const StringRef code,
                                  const StringRef output) {
  auto bufferPtr = MemoryBuffer::getMemBuffer(code, "source");
  const auto &buffer = *bufferPtr;
  SourceMgr srcMgr;
  const auto id = srcMgr.AddNewSourceBuffer(std::move(bufferPtr), {});
  Lexer lexer(srcMgr, buffer.getBuffer());
  if (const auto success = lexer.Lex(); !success)
    return false;

  Parser parser(lexer.getTokens(), srcMgr);
  const auto program = parser.Parse();
  if (parser.getError())
    return false;

  std::string result;
  raw_string_ostream ss(result);
  LoxValueEnv Env;
  ExprInterpreter exprInterpreter(srcMgr, Env);
  StmtInterpreter stmtInterpreter(srcMgr, exprInterpreter, Env, ss);
  {
    LoxValueScope globalScope(Env);
    for (const auto &stmt : program)
      std::visit(stmtInterpreter, *stmt);
  }

  if (stmtInterpreter.getError())
    return false;

  if (result != output) {
    FAIL(result);
    return false;
  }

  return true;
}

#define PRINT_OUTPUT_TEST(test, code, expect)                                  \
  SUBCASE(test) CHECK(::lox::runPrintInterpretTest(code, expect))

} // namespace lox

#endif /// __INTERPRETER_TEST_UTILS_HPP__