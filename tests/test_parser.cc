#include <gtest/gtest.h>

#include "toml_cpp/node.h"
#include "toml_cpp/parser.h"

TEST(ParserTest, test) {
  const std::vector tokens = {
      {toml::TokenType::TOKEN_KEY, "name"},
      toml::tokens::EQUAL,
      {toml::TokenType::TOKEN_STRING, "Orange"},
      {toml::TokenType::TOKEN_KEY, "physical"},
      toml::tokens::DOT,
      {toml::TokenType::TOKEN_KEY, "color"},
      toml::tokens::EQUAL,
      {toml::TokenType::TOKEN_STRING, "orange"},
      {toml::TokenType::TOKEN_KEY, "physical"},
      toml::tokens::DOT,
      {toml::TokenType::TOKEN_KEY, "shape"},
      toml::tokens::EQUAL,
      {toml::TokenType::TOKEN_STRING, "round"},
      {toml::TokenType::TOKEN_KEY, "site"},
      toml::tokens::DOT,
      {toml::TokenType::TOKEN_KEY, "google.com"},
      toml::tokens::EQUAL,
      {toml::TokenType::TOKEN_BOOLEAN, "true"},
      // Tables
      toml::tokens::LBRACKET,
      {toml::TokenType::TOKEN_KEY, "table"},
      toml::tokens::RBRACKET,

      toml::tokens::LBRACKET,
      {toml::TokenType::TOKEN_KEY, "table-1"},
      toml::tokens::RBRACKET,
      {toml::TokenType::TOKEN_KEY, "key1"},
      toml::tokens::EQUAL,
      {toml::TokenType::TOKEN_STRING, "some string"},
      {toml::TokenType::TOKEN_KEY, "key2"},
      toml::tokens::EQUAL,
      {toml::TokenType::TOKEN_INTEGER, "123"},

      toml::tokens::LBRACKET,
      {toml::TokenType::TOKEN_KEY, "table-2"},
      toml::tokens::RBRACKET,
      {toml::TokenType::TOKEN_KEY, "key1"},
      toml::tokens::EQUAL,
      {toml::TokenType::TOKEN_STRING, "another string"},
      {toml::TokenType::TOKEN_KEY, "key2"},
      toml::tokens::EQUAL,
      {toml::TokenType::TOKEN_INTEGER, "456"},

      toml::tokens::LBRACKET,
      {toml::TokenType::TOKEN_KEY, "dog"},
      toml::tokens::DOT,
      {toml::TokenType::TOKEN_KEY, "tater.man"},
      toml::tokens::RBRACKET,
      {toml::TokenType::TOKEN_KEY, "type"},
      toml::tokens::DOT,
      {toml::TokenType::TOKEN_KEY, "name"},
      toml::tokens::EQUAL,
      {toml::TokenType::TOKEN_STRING, "pug"},

      // Offset Date-Time
      {toml::TokenType::TOKEN_KEY, "odt1"},
      toml::tokens::EQUAL,
      {toml::TokenType::TOKEN_OFFSET_DATETIME, "1979-05-27T07:32:00Z"},
      {toml::TokenType::TOKEN_KEY, "odt2"},
      toml::tokens::EQUAL,
      {toml::TokenType::TOKEN_OFFSET_DATETIME, "1979-05-27T00:32:00-07:00"},
      {toml::TokenType::TOKEN_KEY, "odt3"},
      toml::tokens::EQUAL,
      {toml::TokenType::TOKEN_OFFSET_DATETIME, "1979-05-27T00:32:00.999999-07:00"},
      {toml::TokenType::TOKEN_KEY, "odt4"},
      toml::tokens::EQUAL,
      {toml::TokenType::TOKEN_OFFSET_DATETIME, "1979-05-27 07:32:00Z"},

      // Local Date-Time
      {toml::TokenType::TOKEN_KEY, "ldt1"},
      toml::tokens::EQUAL,
      {toml::TokenType::TOKEN_LOCAL_DATETIME, "1979-05-27T07:32:00"},
      {toml::TokenType::TOKEN_KEY, "ldt2"},
      toml::tokens::EQUAL,
      {toml::TokenType::TOKEN_LOCAL_DATETIME, "1979-05-27T00:32:00.999999"},

      // Local Date
      {toml::TokenType::TOKEN_KEY, "ld1"},
      toml::tokens::EQUAL,
      {toml::TokenType::TOKEN_LOCAL_DATE, "1979-05-27"},

      // Local Time
      {toml::TokenType::TOKEN_KEY, "lt1"},
      toml::tokens::EQUAL,
      {toml::TokenType::TOKEN_LOCAL_TIME, "07:32:00"},
      {toml::TokenType::TOKEN_KEY, "lt2"},
      toml::tokens::EQUAL,
      {toml::TokenType::TOKEN_LOCAL_TIME, "00:32:00.999999"},
      toml::tokens::END_OF_FILE,
  };

  toml::Parser parser(tokens);
  const toml::Node root_node = parser.Parse();

  std::cout << "--- Verifying Parsed TOML Data (JSON-like output) ---" << std::endl;
  toml::PrintNode(root_node);
  std::cout << std::endl;

  ASSERT_TRUE(false);
}
