#include "parser.hpp"

#include <iostream>

namespace toml {
parser::parser(std::vector<token> tokens) : tokens_(std::move(tokens)) {
}

Node parser::Parse() {
  auto root_table = std::map<std::string, std::shared_ptr<Node> >();

  while (pos_ < tokens_.size() && tokens_[pos_].type != TokenType::TOKEN_EOF) {
    if (tokens_[pos_].type != TokenType::TOKEN_KEY) {
      pos_++; // Skip non-key tokens like newlines.
      continue;
    }

    // --- We found a key, so we expect a key path (e.g., key1.key2) ---
    std::vector<std::string> key_parts;
    key_parts.push_back(tokens_[pos_].literal);
    pos_++;

    // Consume subsequent '.key' parts
    while (pos_ < tokens_.size() && tokens_[pos_].type == TokenType::TOKEN_DOT) {
      pos_++; // Consume the dot
      if (pos_ >= tokens_.size() || tokens_[pos_].type != TokenType::TOKEN_KEY) {
        std::cerr << "Parser Error: Expected a key after '.'." << std::endl;
        // In a real parser, you'd throw an exception. Here we stop.
        return Node(root_table);
      }
      key_parts.push_back(tokens_[pos_].literal);
      pos_++;
    }

    // --- We have the full key path, now expect '=' ---
    if (pos_ >= tokens_.size() || tokens_[pos_].type != TokenType::TOKEN_EQUAL) {
      std::cerr << "Parser Error: Expected '=' after key path." << std::endl;
      break;
    }
    pos_++; // Consume '='

    // --- Now expect a value ---
    if (pos_ >= tokens_.size() || tokens_[pos_].type == TokenType::TOKEN_EOF) {
      std::cerr << "Parser Error: Expected a value but found end of input." << std::endl;
      break;
    }

    // --- Parse the value token and create a Node for it ---
    const auto& value_token = tokens_[pos_];
    std::shared_ptr<Node> value_node;
    try {
      switch (value_token.type) {
        case TokenType::TOKEN_STRING:
          value_node = std::make_shared<Node>(value_token.literal);
          break;
        case TokenType::TOKEN_INTEGER:
          value_node = std::make_shared<Node>(std::stoll(value_token.literal));
          break;
        case TokenType::TOKEN_FLOAT:
          value_node = std::make_shared<Node>(std::stod(value_token.literal));
          break;
        case TokenType::TOKEN_BOOLEAN:
          value_node = std::make_shared<Node>(value_token.literal == "true");
          break;
        default:
          std::cerr << "Parser Error: Invalid value token." << std::endl;
          break;
      }
    } catch (const std::exception& e) {
      std::cerr << "Parser Error: Could not convert value '" << value_token.literal << "'. " << e.what() << std::endl;
      value_node = nullptr;
    }

    // --- Navigate or create nested tables and insert the value ---
    if (value_node) {
      auto* current_table_ptr = &root_table;
      bool error = false;

      // Iterate through the path, creating tables as needed.
      for (size_t i = 0; i < key_parts.size() - 1; ++i) {
        const auto& key_part = key_parts[i];

        if (auto it = current_table_ptr->find(key_part); it == current_table_ptr->end()) {
          // Path does not exist, create a new table.
          const auto new_table_node = std::make_shared<Node>(std::map<std::string, std::shared_ptr<Node>>());
          (*current_table_ptr)[key_part] = new_table_node;
          current_table_ptr = &new_table_node->as_table();
        } else if (it->second->is_table()) {
          // Path exists and is a table, traverse into it.
          current_table_ptr = &it->second->as_table();
        } else {
          // Error: a value exists where a table should be.
          std::cerr << "Parser Error: Key '" << key_part << "' is a value, but is used as a table." << std::endl;
          error = true;
          break;
        }
      }

      if (!error) {
        // Insert the final value into the target table.
        const auto& final_key_part = key_parts.back();
        if (current_table_ptr->contains(final_key_part)) {
          std::cerr << "Parser Warning: Redefining key '" << final_key_part << "' in table." << std::endl;
        }
        (*current_table_ptr)[final_key_part] = value_node;
      }
    }

    // Advance past the value to the next token.
    pos_++;
  }

  return Node(root_table);
}
}
