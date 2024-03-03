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

inline bool runParseTest(const StringRef code, const StringRef expected) {
  auto bufferPtr = MemoryBuffer::getMemBuffer(code, "source");
  const auto &buffer = *bufferPtr;
  SourceMgr srcMgr;
  const auto id = srcMgr.AddNewSourceBuffer(std::move(bufferPtr), {});
  Lexer lexer(srcMgr, buffer.getBuffer());
  const auto success = lexer.Lex();
  if (!success)
    return false;

  Parser parser(lexer.getTokens(), srcMgr);
  auto expr = parser.Expression();
  if (!expr)
    return false;

  ExprPrinter exprPrinter;
  std::visit(exprPrinter, *expr);

  if (exprPrinter.getResult() != expected) {
    FAIL(exprPrinter.getResult());
    return false;
  }

  return true;
}

#define PARSER_TEST(test, code, expect)                                        \
  SUBCASE(test) CHECK(::lox::runParseTest(code, expect));

} // namespace lox

#endif // __PARSER_TEST_UTIL_HPP__
