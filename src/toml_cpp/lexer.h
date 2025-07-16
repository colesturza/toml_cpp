#ifndef TOML_CPP_LEXER_H
#define TOML_CPP_LEXER_H

#include <stack>
#include <string>

#include "toml_cpp/token.h"

namespace toml {
/**
 * @class Lexer
 * @brief The lexer, or tokenizer, for the TOML parser.
 *
 * This class takes a raw TOML string as input and breaks it down into a
 * stream of tokens. Each token represents a meaningful unit in the TOML
 * syntax, such as a key, a value, or punctuation.
 */
class Lexer {
 public:
  /**
   * @brief Constructs a Lexer with the given input string.
   * @param input The TOML string to be tokenized.
   */
  explicit Lexer(std::string input);

  /**
   * @brief Reads the next token from the input string.
   * @return The next token in the sequence.
   */
  token next_token();

 private:
  /// @brief The input TOML string being processed.
  std::string input_;
  /// @brief The current character position in the input string.
  size_t pos_ = 0;
  /// @brief The next character position to be read (always pos_ + 1).
  size_t read_pos_ = 0;
  /// @brief The current character being examined.
  char ch_ = 0;
  /// @brief Flag to indicate if the lexer expects the next token to be a key.
  bool expect_key_ch_ = true;
  /// @brief Stack to manage nested structures like inline tables and arrays.
  std::stack<char> scope_stack_;

  /**
   * @brief Reads the next character from the input and advances the position.
   */
  void read_char();

  /**
   * @brief Peeks at a character 'n' positions ahead without consuming it.
   * @param n The number of characters to look ahead.
   * @return The character at the specified future position, or 0 if out of
   * bounds.
   */
  [[nodiscard]] char peek_ahead_char(size_t n = 1) const;

  /**
   * @brief Skips over any whitespace characters.
   */
  void skip_white_space();

  /**
   * @brief Skips over a comment line (from '#' to newline).
   */
  void skip_comment();

  /**
   * @brief Skips any consecutive whitespace and comments.
   */
  void skip_whitespace_and_comments();

  /**
   * @brief Reads a TOML basic string enclosed in double quotes.
   * @return The parsed content of the string.
   */
  std::string read_basic_string();

  /**
   * @brief Reads a TOML literal string enclosed in single quotes.
   * @return The parsed content of the string.
   */
  std::string read_literal_string();

  /**
   * @brief Reads a TOML multiline basic string enclosed in triple double
   * quotes.
   * @return The parsed content of the string.
   */
  std::string read_multiline_basic_string();

  /**
   * @brief Reads a TOML multiline literal string enclosed in triple single
   * quotes.
   * @return The parsed content of the string.
   */
  std::string read_multiline_literal_string();

  /**
   * @brief Reads a key token, which can be bare, quoted, or dotted.
   * @return The token representing the key.
   */
  token read_key();

  /**
   * @brief Reads a bare (unquoted) key.
   * @return The string content of the bare key.
   */
  std::string read_bare_key();

  /**
   * @brief Reads a key enclosed in quotes.
   * @param quote_char The type of quote (' or ") used.
   * @return The token representing the quoted key.
   */
  token read_quoted_key(char quote_char);

  /**
   * @brief Reads a numerical value (integer or float).
   * @return The token representing the number.
   */
  token read_number();

  /**
   * @brief Reads a bare word, which could be a boolean or a component of a
   * date/time.
   * @return The string content of the bare word.
   */
  std::string read_bare_word();

  /**
   * @brief Reads a date-time or local time value.
   * @return The token representing the date-time value.
   */
  token read_date_time();

  /**
   * @brief Checks if the current position looks like the start of a date-time
   * value.
   * @return True if it resembles a date-time, false otherwise.
   */
  [[nodiscard]] bool is_date_time_like() const;

  /**
   * @brief Checks if a character is valid within a bare key (after the start).
   * @param c The character to check.
   * @return True if the character is valid, false otherwise.
   */
  static bool is_valid_key_char(char c);

  /**
   * @brief Checks if a character is valid for a bare word (like 'true',
   * 'false', '+inf', 'nan', etc.).
   * @param c The character to check.
   * @return True if the character is valid, false otherwise.
   */
  static bool is_valid_char_for_bare_word(char c);
};
}  // namespace toml

#endif  // TOML_CPP_LEXER_H
