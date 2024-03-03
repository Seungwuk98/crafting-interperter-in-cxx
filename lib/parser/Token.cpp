#include "lox/parser/Token.hpp"

static const char *TokenKindNames[] = {
#define TOKEN(ID) #ID,
#include "lox/parser/Token.def"

};

namespace lox {

const char *getTokenName(const TokenKind kind) {
  return TokenKindNames[static_cast<unsigned>(kind)];
}
} // namespace lox