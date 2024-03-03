#ifndef __LOX_TOKEN_HPP__
#define __LOX_TOKEN_HPP__

#include "llvm/Support/SMLoc.h"

#include <llvm/ADT/StringRef.h>

namespace lox {
using namespace llvm;

enum TokenKind : unsigned {
#define TOKEN(ID) Tok_##ID,
#include "lox/parser/Token.def"

  TokenCount
};

const char *getTokenName(TokenKind kind);

struct Token {
  SMLoc Loc;
  std::size_t Index;
  TokenKind Kind;
  StringRef Symbol;

  Token(const SMLoc &loc, const std::size_t index, const TokenKind kind,
        const StringRef symbol)
      : Loc(loc), Index(index), Kind(kind), Symbol(symbol) {}
};


} // namespace lox

#endif // __LOX_TOKEN_HPP__
