#ifndef TOML_CPP_PARSER_H
#define TOML_CPP_PARSER_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "toml_cpp/node.h"
#include "toml_cpp/token.h"

namespace toml {
using Table = std::map<std::string, std::shared_ptr<Node>>;

class Parser {
 public:
  /**
   * @brief Constructs a parser with a vector of tokens.
   * @param tokens The sequence of tokens to be parsed.
   */
  explicit Parser(std::vector<token> tokens);

  /**
   * @brief Parses the tokens and builds the abstract syntax tree.
   * @return The root node of the parsed TOML structure, which is a table.
   */
  Node Parse();

 private:
  /**
   * @brief Processes a table header token sequence, e.g., `[server.database]`.
   * This method finds or creates the specified table and updates the current
   * table context.
   * @param root_table A reference to the root table of the entire document.
   * @param current_table_ptr A reference to the pointer that tracks the current
   * table context.
   */
  void ProcessTableHeader(Table &root_table, Table *&current_table_ptr);

  /**
   * @brief Processes a key-value pair token sequence, e.g., `key = "value"`.
   * This method parses the key path and the value, then inserts the value
   * into the appropriate table.
   * @param current_table_ptr A reference to the pointer that tracks the current
   * table context.
   */
  void ProcessKeyValuePair(Table *&current_table_ptr);

  /**
   * @brief Parses a potentially dotted key path (e.g., `key1.key2`).
   * @return A vector of strings representing the parts of the key path.
   */
  std::vector<std::string> ParseKeyPath();

  /**
   * @brief Parses a value token (string, integer, float, boolean).
   * @return A shared pointer to a new Node containing the parsed value.
   */
  std::shared_ptr<Node> ParseValue();

  /**
   * @brief Finds a table specified by a path, creating it if it doesn't exist.
   * @param base_table The table to start the search from (usually the root).
   * @param path The parts of the key path leading to the target table.
   * @return A pointer to the found or created table, or nullptr on error.
   */
  static Table *FindOrCreateTable(Table &base_table, const std::vector<std::string> &path);

  std::vector<token> tokens_;
  size_t pos_ = 0;
};
}  // namespace toml

#endif  // TOML_CPP_PARSER_H
