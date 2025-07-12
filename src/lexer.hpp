#ifndef TOML_CPP_LEXER_HPP
#define TOML_CPP_LEXER_HPP

#include <stack>
#include <string>
#include "token.hpp"
#include "unicode.hpp"

namespace toml {
class lexer {
public:
  /**
   * @brief Constructs a Lexer with the given input string.
   * @param input The TOML string to be tokenized.
   */
  explicit lexer(std::string input);

  /**
   * @brief Reads the next token from the input string.
   * @return The next token.
   */
  token next_token();

private:
  std::string input_;
  size_t pos_ = 0;
  size_t read_pos_ = 0;
  char ch_ = 0;
  bool expect_key_ch_ = true;
  std::stack<char> scope_stack_;

  // Core Functions
  void read_char();

  [[nodiscard]] char peek_char() const;

  [[nodiscard]] char peek_ahead_char(size_t n) const;

  // Skipping Functions
  void skip_white_space();

  void skip_comment();

  void skip_whitespace_and_comments();

  // String Reading Functions
  std::string read_basic_string();

  std::string read_literal_string();

  std::string read_multiline_basic_string();

  std::string read_multiline_literal_string();

  // Key Reading Functions
  token read_key();

  std::string read_bare_key();

  token read_quoted_key(char quote_char);

  // Other Value Reading Functions
  token read_number();

  std::string read_bare_word();

  token read_date_time();

  [[nodiscard]] bool is_date_time_like() const;

  // Character Validation Functions
  static bool is_valid_key_start(char c);

  static bool is_valid_key_char(char c);

  static bool is_valid_char_for_bare_word(char c);
};
} // namespace toml

#endif // TOML_CPP_LEXER_HPP
