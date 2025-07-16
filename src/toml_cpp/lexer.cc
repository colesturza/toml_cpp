#include "lexer.h"

#include <cctype>
#include <stdexcept>

#include "toml_cpp/unicode.h"

namespace toml {
Lexer::Lexer(std::string input) : input_(std::move(input)) {
  read_char();
}

void Lexer::read_char() {
  if (read_pos_ >= input_.length()) {
    ch_ = '\0';  // End of input
  } else {
    ch_ = input_[read_pos_];
  }
  pos_ = read_pos_;
  read_pos_++;
}

char Lexer::peek_ahead_char(const size_t n) const {
  const size_t idx = read_pos_ + n - 1;
  return idx < input_.size() ? input_[idx] : '\0';
}

void Lexer::skip_white_space() {
  while (std::isspace(ch_)) {
    read_char();
  }
}

void Lexer::skip_comment() {
  if (ch_ == '#') {
    while (ch_ != '\n' && ch_ != '\0') {
      read_char();
    }
  }
}

void Lexer::skip_whitespace_and_comments() {
  while (true) {
    if (std::isspace(ch_)) {
      skip_white_space();
    } else if (ch_ == '#') {
      skip_comment();
    } else {
      break;
    }
  }
}

std::string Lexer::read_basic_string() {
  std::string result;
  read_char();  // Consume opening quote
  while (ch_ != '"') {
    if (ch_ == '\\') {
      read_char();  // Consume backslash
      switch (ch_) {
        case 'b':
          result += '\b';
          break;
        case 't':
          result += '\t';
          break;
        case 'n':
          result += '\n';
          break;
        case 'f':
          result += '\f';
          break;
        case 'r':
          result += '\r';
          break;
        case '"':
          result += '"';
          break;
        case '\\':
          result += '\\';
          break;
        case 'u': {
          std::string hex;
          for (int i = 0; i < 4; ++i) {
            read_char();
            if (!isxdigit(ch_))
              throw std::runtime_error("Invalid Unicode escape sequence in string.");
            hex += ch_;
          }
          result += util::unicode_codepoint(hex, 4);
          break;
        }
        case 'U': {
          std::string hex;
          for (int i = 0; i < 8; ++i) {
            read_char();
            if (!isxdigit(ch_))
              throw std::runtime_error("Invalid Unicode escape sequence in string.");
            hex += ch_;
          }
          result += util::unicode_codepoint(hex, 8);
          break;
        }
        default:
          throw std::runtime_error("Invalid escape sequence in string.");
      }
    } else if (ch_ == '\0') {
      throw std::runtime_error("Unterminated string.");
    } else {
      result += ch_;
    }
    read_char();
  }
  read_char();  // Consume closing quote
  return result;
}

std::string Lexer::read_literal_string() {
  read_char();  // Consume opening quote
  const size_t start_pos = pos_;
  while (ch_ != '\'' && ch_ != '\0') {
    read_char();
  }
  if (ch_ == '\0') {
    throw std::runtime_error("Unterminated literal string.");
  }
  std::string result = input_.substr(start_pos, pos_ - start_pos);
  read_char();  // Consume closing quote
  return result;
}

std::string Lexer::read_multiline_basic_string() {
  std::string result;
  if (ch_ == '\n') {
    // Skip first newline if present
    read_char();
  } else if (ch_ == '\r' && peek_ahead_char() == '\n') {
    // Handle CRLF
    read_char();
    read_char();
  }

  while (true) {
    if (ch_ == '\0') throw std::runtime_error("Unterminated multi-line basic string.");
    if (ch_ == '"' && peek_ahead_char() == '"' && peek_ahead_char(2) == '"') {
      read_char();
      read_char();
      read_char();  // Consume """
      break;
    }

    if (ch_ == '\\') {
      read_char();  // Consume backslash
      if (ch_ == '\n' || std::isspace(ch_)) {
        // Trim whitespace after line-ending backslash
        while (std::isspace(ch_)) read_char();
        continue;
      }
      switch (ch_) {
        case 'b':
          result += '\b';
          break;
        case 't':
          result += '\t';
          break;
        case 'n':
          result += '\n';
          break;
        case 'f':
          result += '\f';
          break;
        case 'r':
          result += '\r';
          break;
        case '"':
          result += '"';
          break;
        case '\\':
          result += '\\';
          break;
        case 'u': {
          std::string hex;
          for (int i = 0; i < 4; ++i) {
            read_char();
            if (!isxdigit(ch_))
              throw std::runtime_error("Invalid Unicode escape sequence in string.");
            hex += ch_;
          }
          result += util::unicode_codepoint(hex, 4);
          break;
        }
        case 'U': {
          std::string hex;
          for (int i = 0; i < 8; ++i) {
            read_char();
            if (!isxdigit(ch_))
              throw std::runtime_error("Invalid Unicode escape sequence in string.");
            hex += ch_;
          }
          result += util::unicode_codepoint(hex, 8);
          break;
        }
        default:
          throw std::runtime_error("Invalid escape sequence in multi-line string.");
      }
    } else {
      result += ch_;
    }
    read_char();
  }
  return result;
}

std::string Lexer::read_multiline_literal_string() {
  std::string result;
  if (ch_ == '\n') {
    // Skip first newline if present
    read_char();
  } else if (ch_ == '\r' && peek_ahead_char() == '\n') {
    // Handle CRLF
    read_char();
    read_char();
  }

  while (true) {
    if (ch_ == '\0') throw std::runtime_error("Unterminated multi-line literal string.");
    if (ch_ == '\'' && peek_ahead_char() == '\'' && peek_ahead_char(2) == '\'') {
      read_char();
      read_char();
      read_char();  // Consume '''
      break;
    }
    result += ch_;
    read_char();
  }
  return result;
}

std::string Lexer::read_bare_word() {
  const size_t start_pos = pos_;
  while (is_valid_char_for_bare_word(ch_)) {
    read_char();
  }
  return input_.substr(start_pos, pos_ - start_pos);
}

token Lexer::read_number() {
  const size_t start_pos = pos_;
  bool is_float = false;

  if (ch_ == '+' || ch_ == '-') read_char();

  if (ch_ == '0' &&
      (peek_ahead_char() == 'x' || peek_ahead_char() == 'o' || peek_ahead_char() == 'b')) {
    read_char();
    read_char();  // consume '0' and base specifier
    while (std::isxdigit(ch_) || ch_ == '_') read_char();
    return {TokenType::TOKEN_INTEGER, input_.substr(start_pos, pos_ - start_pos)};
  }

  while (std::isdigit(ch_) || ch_ == '_') read_char();

  if (ch_ == '.' && std::isdigit(peek_ahead_char())) {
    is_float = true;
    read_char();  // consume '.'
    while (std::isdigit(ch_) || ch_ == '_') read_char();
  }

  if (ch_ == 'e' || ch_ == 'E') {
    is_float = true;
    read_char();  // consume 'e' or 'E'
    if (ch_ == '+' || ch_ == '-') read_char();
    while (std::isdigit(ch_) || ch_ == '_') read_char();
  }

  return {is_float ? TokenType::TOKEN_FLOAT : TokenType::TOKEN_INTEGER,
          input_.substr(start_pos, pos_ - start_pos)};
}

std::string Lexer::read_bare_key() {
  const size_t start_pos = pos_;
  while (is_valid_key_char(ch_)) {
    read_char();
  }
  return input_.substr(start_pos, pos_ - start_pos);
}

token Lexer::read_quoted_key(const char quote_char) {
  read_char();  // consume opening quote
  const size_t start_pos = pos_;

  while (ch_ != quote_char && ch_ != '\0') {
    if (quote_char == '"' && ch_ == '\\') {
      read_char();  // consume '\' and the escaped character
    }
    read_char();
  }
  if (ch_ == '\0') throw std::runtime_error("Unterminated quoted key.");

  const std::string literal = input_.substr(start_pos, pos_ - start_pos);
  read_char();  // consume closing quote
  return {TokenType::TOKEN_KEY, literal};
}

token Lexer::read_key() {
  skip_whitespace_and_comments();

  if (ch_ == '"' || ch_ == '\'') {
    return read_quoted_key(ch_);
  }

  if (is_valid_key_char(ch_)) {
    const std::string key_literal = read_bare_key();
    return {TokenType::TOKEN_KEY, key_literal};
  }

  return {TokenType::TOKEN_ILLEGAL, std::string(1, ch_)};
}

token Lexer::next_token() {
  skip_whitespace_and_comments();
  token tok;

  if (expect_key_ch_) {
    if (ch_ == '[') {
      if (peek_ahead_char() == '[') {
        tok = tokens::LDBRACKET;
        read_char();
        read_char();
        return tok;
      }
      read_char();
      return tokens::LBRACKET;
    }
    expect_key_ch_ = false;
    tok = read_key();
    if (tok.type == TokenType::TOKEN_ILLEGAL || ch_ == '\0') {
      tok = tokens::END_OF_FILE;
    }
    return tok;
  }

  switch (ch_) {
    case '=':
      read_char();
      return tokens::EQUAL;

    case '.':
      read_char();
      expect_key_ch_ = true;
      return tokens::DOT;

    case '[':
      scope_stack_.push('[');  // Push array context
      read_char();
      return tokens::LBRACKET;

    case ']':
      if (peek_ahead_char() == ']') {
        read_char();
        read_char();
        expect_key_ch_ = true;
        return tokens::RDBRACKET;
      }
      if (!scope_stack_.empty() && scope_stack_.top() == '[') {
        scope_stack_.pop();  // Pop array context
      }
      read_char();
      // A key is expected if we are no longer in a nested structure.
      expect_key_ch_ = scope_stack_.empty();
      return tokens::RBRACKET;

    case '{':
      scope_stack_.push('{');  // Push inline table context
      read_char();
      expect_key_ch_ = true;
      return tokens::LBRACE;

    case '}':
      if (!scope_stack_.empty() && scope_stack_.top() == '{') {
        scope_stack_.pop();  // Pop inline table context
      }
      read_char();
      // A key is expected if we are no longer in a nested structure.
      expect_key_ch_ = scope_stack_.empty();
      return tokens::RBRACE;

    case ',':
      read_char();
      // A key is expected after a comma only if we're in an inline table.
      expect_key_ch_ = !scope_stack_.empty() && scope_stack_.top() == '{';
      return tokens::COMMA;

    case '\0':
      return tokens::END_OF_FILE;

    case '"':
      if (peek_ahead_char() == '"' && peek_ahead_char(2) == '"') {
        read_char();
        read_char();
        read_char();  // consume """
        tok = {TokenType::TOKEN_STRING, read_multiline_basic_string()};
      } else {
        tok = {TokenType::TOKEN_STRING, read_basic_string()};
      }
      break;

    case '\'':
      if (peek_ahead_char() == '\'' && peek_ahead_char(2) == '\'') {
        read_char();
        read_char();
        read_char();  // consume '''
        tok = {TokenType::TOKEN_STRING, read_multiline_literal_string()};
      } else {
        tok = {TokenType::TOKEN_STRING, read_literal_string()};
      }
      break;

    default:
      if (is_date_time_like()) {
        tok = read_date_time();
      } else if (std::isalpha(ch_)) {
        if (const std::string literal = read_bare_word(); literal == "true" || literal == "false") {
          tok = {TokenType::TOKEN_BOOLEAN, literal};
        } else if (literal == "inf" || literal == "nan") {
          tok = {TokenType::TOKEN_FLOAT, literal};
        } else {
          tok = {TokenType::TOKEN_ILLEGAL, literal};
        }
      } else if (std::isdigit(ch_) || ((ch_ == '+' || ch_ == '-') &&
                                       (std::isdigit(peek_ahead_char()) ||
                                        peek_ahead_char() == 'i' || peek_ahead_char() == 'n'))) {
        if (peek_ahead_char() == 'i' || peek_ahead_char() == 'n') {
          // +inf, -inf, +nan, -nan
          const std::string literal = read_bare_word();
          tok = {TokenType::TOKEN_FLOAT, literal};
        } else {
          tok = read_number();
        }
      } else {
        tok = {TokenType::TOKEN_ILLEGAL, std::string(1, ch_)};
        read_char();
      }
      break;
  }

  // After a value, a key is expected only if we are at the top-level scope.
  // Inside an array or inline table, the next token should be a comma or
  // closing delimiter.
  expect_key_ch_ = scope_stack_.empty();

  return tok;
}

/**
 * @brief Peeks ahead to see if the input looks like a date or time.
 * This is the crucial lookahead step to distinguish dates from integers.
 */
bool Lexer::is_date_time_like() const {
  // A simple check: if it starts with 4 digits and a dash, it's a date.
  if (std::isdigit(ch_) && std::isdigit(peek_ahead_char(1)) && std::isdigit(peek_ahead_char(2)) &&
      std::isdigit(peek_ahead_char(3)) && peek_ahead_char(4) == '-') {
    return true;  // Likely a Local Date or Date-Time
  }

  // If it starts with 2 digits and a colon, it's a time.
  if (std::isdigit(ch_) && std::isdigit(peek_ahead_char(1)) && peek_ahead_char(2) == ':') {
    return true;  // Likely a Local Time
  }

  return false;
}

/**
 * @brief Reads a date-time, local date, or local time token.
 * @return A token representing the parsed value.
 */
token Lexer::read_date_time() {
  const size_t start_pos = pos_;
  bool has_date = false;
  bool has_time = false;
  bool has_t_separator = false;
  bool has_offset = false;

  if (std::isdigit(ch_) && peek_ahead_char(4) == '-') {
    has_date = true;
  }

  // Consume the entire date/time/offset sequence
  while (std::isdigit(ch_) || ch_ == '-' || ch_ == 'T' || ch_ == 't' || ch_ == ' ' || ch_ == ':' ||
         ch_ == 'Z' || ch_ == 'z' || ch_ == '+' || ch_ == '.') {
    if (ch_ == 'T' || ch_ == 't' || ch_ == ' ') has_t_separator = true;
    if (ch_ == ':') has_time = true;
    if (ch_ == '+' || (ch_ == '-' && has_time)) has_offset = true;
    if (ch_ == 'Z' || ch_ == 'z') has_offset = true;

    read_char();
  }

  const std::string literal = input_.substr(start_pos, pos_ - start_pos);

  if (has_date && has_t_separator && has_offset) {
    return {TokenType::TOKEN_OFFSET_DATETIME, literal};
  }
  if (has_date && has_t_separator) {
    return {TokenType::TOKEN_LOCAL_DATETIME, literal};
  }
  if (has_date) {
    return {TokenType::TOKEN_LOCAL_DATE, literal};
  }
  if (has_time) {
    return {TokenType::TOKEN_LOCAL_TIME, literal};
  }

  return {TokenType::TOKEN_ILLEGAL, literal};
}

bool Lexer::is_valid_key_char(const char c) {
  return std::isalpha(c) || std::isdigit(c) || c == '_' || c == '-';
}

bool Lexer::is_valid_char_for_bare_word(const char c) {
  return std::isalpha(c) || std::isdigit(c) || c == '_' || c == '-' || c == '+';
}
}  // namespace toml
