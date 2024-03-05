#include "lox/parser/Lexer.hpp"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Support/FormatVariadic.h"
#include "llvm/Support/raw_ostream.h"
#include <cassert>

namespace lox {

bool Lexer::Lex() {
  auto tok = addNextToken();
  while (tok.Kind != Tok_eof) {
    tok = addNextToken();
  }
  if (errorNum)
    return false;
  return true;
}

#define RETURN_TOKEN(kind)                                                     \
  {                                                                            \
    Token tok{loc, tokens.size(), kind, buffer.slice(curr, cursor)};           \
    tokens.emplace_back(mkuptr<Token>(tok));                                   \
    return tok;                                                                \
  }
Token Lexer::addNextToken() {
lex:
  eatWhitespace();
  auto ch = peekChar();
  const auto curr = cursor;
  const auto loc = SMLoc::getFromPointer(buffer.data() + curr);
  switch (ch) {
  case '(':
    skip();
    RETURN_TOKEN(Tok_lparen);
  case ')':
    skip();
    RETURN_TOKEN(Tok_rparen);
  case '{':
    skip();
    RETURN_TOKEN(Tok_lbrace);
  case '}':
    skip();
    RETURN_TOKEN(Tok_rbrace);
  case ',':
    skip();
    RETURN_TOKEN(Tok_comma);
  case '.':
    skip();
    RETURN_TOKEN(Tok_dot);
  case '+':
    skip();
    RETURN_TOKEN(Tok_plus);
  case '-':
    skip();
    RETURN_TOKEN(Tok_minus);
  case ';':
    skip();
    RETURN_TOKEN(Tok_semicolon);
  case '*':
    skip();
    RETURN_TOKEN(Tok_star);
  case '/':
    skip();
    ch = peekChar();
    if (ch == '/') {
      eatLineComment();
      goto lex;
    } else
      RETURN_TOKEN(Tok_slash);
  case '!':
    skip();
    ch = peekChar();
    if (ch == '=') {
      skip();
      RETURN_TOKEN(Tok_bang_equal);
    } else
      RETURN_TOKEN(Tok_bang);
  case '=':
    skip();
    ch = peekChar();
    if (ch == '=') {
      skip();
      RETURN_TOKEN(Tok_equal_equal);
    } else
      RETURN_TOKEN(Tok_equal);
  case '>':
    skip();
    ch = peekChar();
    if (ch == '=') {
      skip();
      RETURN_TOKEN(Tok_ge);
    } else
      RETURN_TOKEN(Tok_gt);
  case '<':
    skip();
    ch = peekChar();
    if (ch == '=') {
      skip();
      RETURN_TOKEN(Tok_le);
    } else
      RETURN_TOKEN(Tok_lt);
  case CharEof: {
    Token tok{loc, tokens.size(), Tok_eof, ""};
    tokens.emplace_back(mkuptr<Token>(tok));
    return tok;
  }
  case '"': {
    skip();
    auto start = cursor;
    ch = peekChar();
    while (ch != CharEof && ch != '"') {
      skip();
      ch = peekChar();
    }
    if (ch == CharEof) {
      report(loc, SourceMgr::DiagKind::DK_Error, "Unclosed string literal");
      RETURN_TOKEN(Tok_error);
    }
    auto end = cursor;
    skip();
    Token tok{loc, tokens.size(), Tok_string, buffer.slice(start, end)};
    tokens.emplace_back(mkuptr<Token>(tok));
    return tok;
  }
  default:
    if (isNumber(ch)) {
      skip();
      ch = peekChar();
      while (isNumber(ch)) {
        skip();
        ch = peekChar();
      }

      if (ch == '.') {
        skip();
        ch = peekChar();
        while (isNumber(ch)) {
          skip();
          ch = peekChar();
        }
      }
      if (isLetter(ch)) {
        skip();
        ch = peekChar();
        while (!isWhitespace(ch) && ch != CharEof) {
          skip();
          ch = peekChar();
        }
        report(
            loc, SourceMgr::DK_Error,
            llvm::formatv("Unhandled Token : {0}", buffer.slice(curr, cursor))
                .str());
        RETURN_TOKEN(Tok_error);
      }

      RETURN_TOKEN(Tok_number);
    } else if (isLetter(ch)) {
      skip();
      ch = peekChar();
      while (isLetter(ch) || isNumber(ch)) {
        skip();
        ch = peekChar();
      }

      auto symbol = buffer.slice(curr, cursor);
      auto kind = llvm::StringSwitch<TokenKind>(symbol)
#define KEYWORD(ID) .Case(#ID, Tok_##ID)
#include "lox/parser/Token.def"

                      .Default(Tok_identifier);
      Token tok{loc, tokens.size(), kind, symbol};
      tokens.emplace_back(mkuptr<Token>(tok));
      return tok;
    }
    skip();
    ch = peekChar();
    while (!isWhitespace(ch) && ch != CharEof) {
      skip();
      ch = peekChar();
    }
    report(loc, SourceMgr::DK_Error,
           llvm::formatv("Unhandled Token : {0}", buffer.slice(curr, cursor))
               .str());
    RETURN_TOKEN(Tok_error);
  }
}

char Lexer::peekChar(const int lookAt) const {
  assert(static_cast<int64_t>(cursor) + lookAt >= 0 &&
         "Can't look at minus cursur");
  if (cursor + lookAt >= buffer.size())
    return CharEof;
  return buffer[cursor + lookAt];
}

char Lexer::getNextChar() {
  if (cursor >= buffer.size())
    return CharEof;
  return buffer[cursor++];
}

void Lexer::skip() { getNextChar(); }

void Lexer::eatLineComment() {
  auto ch = peekChar();
  while (ch != CharEof && ch != '\n') {
    skip();
    ch = peekChar();
  }
  if (ch == '\n')
    skip();
}

void Lexer::eatWhitespace() {
  auto ch = peekChar();
  while (isWhitespace(ch)) {
    skip();
    ch = peekChar();
  }
}

bool Lexer::isLetter(const char ch) {
  return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_';
}
bool Lexer::isNumber(const char ch) { return '0' <= ch && ch <= '9'; }
bool Lexer::isWhitespace(const char ch) {
  return (ch == '\n' || ch == ' ' || ch == '\r' || ch == '\t' || ch == '\f');
}

void Lexer::report(const SMLoc loc, const SourceMgr::DiagKind kind,
                   const StringRef msg, raw_ostream &os) {
  SrcMgr.PrintMessage(os, loc, kind, msg);
  if (kind == SourceMgr::DiagKind::DK_Error)
    errorNum++;
}

} // namespace lox