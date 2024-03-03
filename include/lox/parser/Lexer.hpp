#ifndef __LEXER_HPP__
#define __LEXER_HPP__

#include "lox/parser/Token.hpp"
#include "utils/TypeUtils.hpp"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include <memory>

namespace lox {
using namespace llvm;

class Lexer {
public:
  Lexer(SourceMgr &S, const StringRef code)
      : SrcMgr(S), buffer(code), cursor(0u), errorNum(0u) {}

  SourceMgr &SrcMgr;

  bool Lex();

  static constexpr char CharEof = static_cast<char>(EOF);

  ArrayRef<uptr<Token>> getTokens() const { return tokens; }

private:
  Token addNextToken();

  [[nodiscard]] char peekChar(int lookAt = 0) const;

  char getNextChar();

  void skip();

  void eatLineComment();

  void eatWhitespace();

  static bool isLetter(char ch);

  static bool isNumber(char ch);

  static bool isWhitespace(char ch);

  void report(SMLoc loc, SourceMgr::DiagKind kind, StringRef msg,
              raw_ostream &os = errs());

  StringRef buffer;
  std::size_t cursor;
  std::size_t errorNum;

  SmallVector<uptr<Token>> tokens;
};
} // namespace lox
#endif
