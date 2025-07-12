#include <gtest/gtest.h>

#include "toml_cpp/node.hpp"
#include "parser.hpp"

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
    toml::tokens::END_OF_FILE,
  };

  toml::parser parser(tokens);
  const toml::Node root_node = parser.Parse();

  std::cout << "--- Verifying Parsed TOML Data (JSON-like output) ---" << std::endl;
  toml::PrintNode(root_node);
  std::cout << std::endl;

  ASSERT_TRUE(false);
}
