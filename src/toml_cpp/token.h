#ifndef TOML_CPP_TOKEN_H
#define TOML_CPP_TOKEN_H

#include <ostream>
#include <string>

namespace toml {
enum class TokenType {
  TOKEN_ILLEGAL,
  TOKEN_EOF,

  // Literals
  TOKEN_KEY,
  TOKEN_STRING,
  TOKEN_INTEGER,
  TOKEN_FLOAT,
  TOKEN_BOOLEAN,
  TOKEN_OFFSET_DATETIME,
  TOKEN_LOCAL_DATETIME,
  TOKEN_LOCAL_DATE,
  TOKEN_LOCAL_TIME,

  // Operators & Punctuation
  TOKEN_EQUAL,
  TOKEN_DOT,
  TOKEN_COMMA,
  TOKEN_LBRACKET,
  TOKEN_RBRACKET,
  TOKEN_LDBRACKET,
  TOKEN_RDBRACKET,
  TOKEN_LBRACE,
  TOKEN_RBRACE,
};

inline std::string to_string(const TokenType type) {
  switch (type) {
    case TokenType::TOKEN_ILLEGAL:
      return "ILLEGAL";
    case TokenType::TOKEN_EOF:
      return "EOF";
    case TokenType::TOKEN_KEY:
      return "KEY";
    case TokenType::TOKEN_STRING:
      return "STRING";
    case TokenType::TOKEN_INTEGER:
      return "INTEGER";
    case TokenType::TOKEN_FLOAT:
      return "FLOAT";
    case TokenType::TOKEN_BOOLEAN:
      return "BOOLEAN";
    case TokenType::TOKEN_OFFSET_DATETIME:
      return "OFFSET_DATETIME";
    case TokenType::TOKEN_LOCAL_DATETIME:
      return "LOCAL_DATETIME";
    case TokenType::TOKEN_LOCAL_DATE:
      return "LOCAL_DATE";
    case TokenType::TOKEN_LOCAL_TIME:
      return "LOCAL_TIME";
    case TokenType::TOKEN_EQUAL:
      return "EQUAL";
    case TokenType::TOKEN_DOT:
      return "DOT";
    case TokenType::TOKEN_COMMA:
      return "COMMA";
    case TokenType::TOKEN_LBRACKET:
      return "LBRACKET";
    case TokenType::TOKEN_RBRACKET:
      return "RBRACKET";
    case TokenType::TOKEN_LDBRACKET:
      return "LDBRACKET";
    case TokenType::TOKEN_RDBRACKET:
      return "RDBRACKET";
    case TokenType::TOKEN_LBRACE:
      return "LBRACE";
    case TokenType::TOKEN_RBRACE:
      return "RBRACE";
    default:
      return "UNKNOWN";
  }
}

struct token {
  TokenType type;
  std::string literal;

  bool operator==(const token &other) const {
    return type == other.type && literal == other.literal;
  }
};

inline std::ostream &operator<<(std::ostream &os, const token &tok) {
  os << "Token{Type: " << to_string(tok.type) << ", literal: \"" << tok.literal << "\"}";
  return os;
}

namespace tokens {
inline constexpr token EQUAL = {TokenType::TOKEN_EQUAL, "="};
inline constexpr token DOT = {TokenType::TOKEN_DOT, "."};
inline constexpr token COMMA = {TokenType::TOKEN_COMMA, ","};
inline constexpr token LBRACKET = {TokenType::TOKEN_LBRACKET, "["};
inline constexpr token RBRACKET = {TokenType::TOKEN_RBRACKET, "]"};
inline constexpr token LDBRACKET = {TokenType::TOKEN_LDBRACKET, "[["};
inline constexpr token RDBRACKET = {TokenType::TOKEN_RDBRACKET, "]]"};
inline constexpr token LBRACE = {TokenType::TOKEN_LBRACE, "{"};
inline constexpr token RBRACE = {TokenType::TOKEN_RBRACE, "}"};
inline constexpr token END_OF_FILE = {TokenType::TOKEN_EOF, ""};
inline constexpr token ILLEGAL = {TokenType::TOKEN_ILLEGAL, "ILLEGAL"};
}  // namespace tokens
}  // namespace toml

#endif  // TOML_CPP_TOKEN_H
