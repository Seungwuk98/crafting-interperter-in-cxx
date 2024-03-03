#ifndef __PARSER_TEST_UTIL_HPP__
#define __PARSER_TEST_UTIL_HPP__

#include "doctest/doctest.h"
#include "lox/ast/ASTPrinter.hpp"
#include "lox/parser/Lexer.hpp"
#include "lox/parser/Parser.hpp"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"

namespace lox {
using namespace llvm;

inline bool runLexTest(const StringRef code, const StringRef expected) {
  auto bufferPtr = MemoryBuffer::getMemBuffer(code, "source");
  const auto &buffer = *bufferPtr;
  SourceMgr srcMgr;
  const auto id = srcMgr.AddNewSourceBuffer(std::move(bufferPtr), {});
  Lexer lexer(srcMgr, buffer.getBuffer());
  const auto success = lexer.Lex();

  std::string lexString;
  llvm::raw_string_ostream ss(lexString);
  for (const auto &token : lexer.getTokens()) {
    auto [row, col] = srcMgr.getLineAndColumn(token->Loc, id);
    ss << row << ":" << col << ":" << lox::getTokenName(token->Kind) << ":"
       << token->Symbol << '\n';
  }

  if (!success || lexString != expected) {
    FAIL(lexString);
    return false;
  }

  return true;
}

#define LEX_TEST(test, code, expect)                                           \
  SUBCASE(test) CHECK(::lox::runLexTest(code, expect));

inline bool runExprParseTest(const StringRef code, const StringRef expected) {
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

  std::string result;
  ExprPrinter exprPrinter(result);
  std::visit(exprPrinter, *expr);

  if (result != expected) {
    FAIL(result);
    return false;
  }

  return true;
}

#define EXPR_PARSE_TEST(test, code, expect)                                    \
  SUBCASE(test) CHECK(::lox::runExprParseTest(code, expect));

inline bool runStmtParseTest(const StringRef code, const StringRef expected) {
  auto bufferPtr = MemoryBuffer::getMemBuffer(code, "source");
  const auto &buffer = *bufferPtr;
  SourceMgr srcMgr;
  const auto id = srcMgr.AddNewSourceBuffer(std::move(bufferPtr), {});
  Lexer lexer(srcMgr, buffer.getBuffer());
  if (const auto success = lexer.Lex(); !success)
    return false;

  Parser parser(lexer.getTokens(), srcMgr);
  auto program = parser.Parse();
  if (parser.getError())
    return false;

  std::string result;
  StmtPrinter stmtPrinter(result);
  for (const auto &stmt : program)
    std::visit(stmtPrinter, *stmt);

  if (result != expected) {
    FAIL(result);
    return false;
  }

  return true;
}

#define PROGRAM_PARSE_TEST(test, code, expect)                                 \
  SUBCASE(test) CHECK(::lox::runStmtParseTest(code, expect));

} // namespace lox

#endif // __PARSER_TEST_UTIL_HPP__
