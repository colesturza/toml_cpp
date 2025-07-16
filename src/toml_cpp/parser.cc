#include "toml_cpp/parser.h"

#include <iostream>

namespace toml {
Parser::Parser(std::vector<token> tokens) : tokens_(std::move(tokens)) {
}

Node Parser::Parse() {
  Table root_table;
  Table *current_table_ptr = &root_table;

  while (pos_ < tokens_.size() && tokens_[pos_].type != TokenType::TOKEN_EOF) {
    if (const auto &[type, literal] = tokens_[pos_]; type == TokenType::TOKEN_LBRACKET) {
      ProcessTableHeader(root_table, current_table_ptr);
    } else if (type == TokenType::TOKEN_KEY) {
      ProcessKeyValuePair(current_table_ptr);
    } else {
      // Skip other tokens like newlines, comments, etc.
      pos_++;
    }
  }

  return Node(root_table);
}

void Parser::ProcessTableHeader(Table &root_table, Table *&current_table_ptr) {
  pos_++;  // Consume '['

  const std::vector<std::string> table_path = ParseKeyPath();

  if (pos_ >= tokens_.size() || tokens_[pos_].type != TokenType::TOKEN_RBRACKET) {
    std::cerr << "Parser Error: Expected ']' to close the table definition." << std::endl;
    // In a real parser, you might want to stop or throw an exception here.
    // For now, we'll just advance to avoid an infinite loop.
    pos_++;
    return;
  }
  pos_++;  // Consume ']'

  if (Table *new_table = FindOrCreateTable(root_table, table_path)) {
    current_table_ptr = new_table;
  }
}

void Parser::ProcessKeyValuePair(Table *&current_table_ptr) {
  std::vector<std::string> key_parts = ParseKeyPath();

  if (pos_ >= tokens_.size() || tokens_[pos_].type != TokenType::TOKEN_EQUAL) {
    std::cerr << "Parser Error: Expected '=' after key path." << std::endl;
    return;
  }
  pos_++;  // Consume '='

  const std::shared_ptr<Node> value_node = ParseValue();
  if (!value_node) {
    // Error already printed in parse_value, just advance and return.
    pos_++;
    return;
  }

  // The last part of the key path is the key for the value, the rest define the
  // table structure.
  const std::string final_key = key_parts.back();
  key_parts.pop_back();

  // Find or create the table for the key, starting from the current context.
  if (Table *target_table = FindOrCreateTable(*current_table_ptr, key_parts)) {
    if (target_table->contains(final_key)) {
      std::cerr << "Parser Error: Redefinition of key '" << final_key << "' is not allowed."
                << std::endl;
    } else {
      (*target_table)[final_key] = value_node;
    }
  }
  pos_++;  // Consume the value token
}

std::vector<std::string> Parser::ParseKeyPath() {
  std::vector<std::string> path;
  if (pos_ >= tokens_.size() || tokens_[pos_].type != TokenType::TOKEN_KEY) {
    std::cerr << "Parser Error: Expected a key." << std::endl;
    return path;
  }

  path.push_back(tokens_[pos_].literal);
  pos_++;

  while (pos_ < tokens_.size() && tokens_[pos_].type == TokenType::TOKEN_DOT) {
    pos_++;  // Consume '.'
    if (pos_ >= tokens_.size() || tokens_[pos_].type != TokenType::TOKEN_KEY) {
      std::cerr << "Parser Error: Expected a key after '.'." << std::endl;
      return path;  // Return partial path on error
    }
    path.push_back(tokens_[pos_].literal);
    pos_++;
  }
  return path;
}

std::shared_ptr<Node> Parser::ParseValue() {
  if (pos_ >= tokens_.size() || tokens_[pos_].type == TokenType::TOKEN_EOF) {
    std::cerr << "Parser Error: Expected a value but found end of input." << std::endl;
    return nullptr;
  }

  const auto &[type, literal] = tokens_[pos_];
  try {
    switch (type) {
      case TokenType::TOKEN_STRING:
        return std::make_shared<Node>(literal);
      case TokenType::TOKEN_INTEGER:
        return std::make_shared<Node>(std::stoll(literal));
      case TokenType::TOKEN_FLOAT:
        return std::make_shared<Node>(std::stod(literal));
      case TokenType::TOKEN_BOOLEAN:
        return std::make_shared<Node>(literal == "true");

      // case TokenType::TOKEN_OFFSET_DATETIME: {
      //   if (auto odt = util::ParseOffsetDateTime(literal)) {
      //     return std::make_shared<Node>(*odt);
      //   }
      //   std::cerr << "Parser Error: Invalid Offset Date-Time format for '" <<
      //   literal << "'." << std::endl; return nullptr;
      // }
      //
      // case TokenType::TOKEN_LOCAL_DATETIME: {
      //   if (auto ldt = util::ParseLocalDateTime(literal)) {
      //     return std::make_shared<Node>(*ldt);
      //   }
      //   std::cerr << "Parser Error: Invalid Local Date-Time format for '" <<
      //   literal << "'." << std::endl; return nullptr;
      // }
      //
      // case TokenType::TOKEN_LOCAL_DATE: {
      //   if (auto ld = util::ParseLocalDate(literal)) {
      //     return std::make_shared<Node>(*ld);
      //   }
      //   std::cerr << "Parser Error: Invalid Local Date format for '" << literal
      //   << "'." << std::endl; return nullptr;
      // }
      //
      // case TokenType::TOKEN_LOCAL_TIME: {
      //   if (auto lt = util::ParseLocalTime(literal)) {
      //     return std::make_shared<Node>(*lt);
      //   }
      //   std::cerr << "Parser Error: Invalid Local Time format for '" << literal
      //   << "'." << std::endl; return nullptr;
      // }
      default:
        std::cerr << "Parser Error: Invalid value token type." << std::endl;
        return nullptr;
    }
  } catch (const std::exception &e) {
    std::cerr << "Parser Error: Could not convert value '" << literal << "'. " << e.what()
              << std::endl;
    return nullptr;
  }
}

Table *Parser::FindOrCreateTable(Table &base_table, const std::vector<std::string> &path) {
  Table *nav_ptr = &base_table;
  for (const auto &path_part : path) {
    if (auto it = nav_ptr->find(path_part); it == nav_ptr->end()) {
      // Path does not exist, create a new table.
      const auto new_table_node = std::make_shared<Node>(Table());
      (*nav_ptr)[path_part] = new_table_node;
      nav_ptr = &new_table_node->as_table();
    } else if (it->second->is_table()) {
      // Path exists and is a table, traverse into it.
      nav_ptr = &it->second->as_table();
    } else {
      // Error: a value exists where a table should be.
      std::cerr << "Parser Error: Key '" << path_part
                << "' is already defined as a value and cannot be a table." << std::endl;
      return nullptr;
    }
  }
  return nav_ptr;
}
}  // namespace toml
