#ifndef TOML_CPP_NODE_H
#define TOML_CPP_NODE_H
#include <iostream>
#include <map>
#include <memory>
#include <variant>
#include <vector>

namespace toml {
class Node;

using NodeValue = std::variant<
  std::string,
  int64_t,
  double,
  bool,
  std::vector<std::shared_ptr<Node> >,
  std::map<std::string, std::shared_ptr<Node> >
>;

/**
 * @class Node
 * @brief A base class for all nodes in the TOML Abstract Syntax Tree (AST).
 *
 * This class uses std::variant to hold different types of TOML data.
 * It provides a unified interface for all data types supported by TOML.
 */
class Node {
public:
  // The actual value of the node, which can be any of the types defined in NodeValue.
  NodeValue value;

  /**
   * @brief Default constructor.
   */
  Node() = default;

  /**
   * @brief Constructor to create a Node with a specific value.
   * @param val The value to be stored in the node.
   */
  explicit Node(NodeValue val) : value(std::move(val)) {
  }

  // Type checking methods

  /**
   * @brief Checks if the node holds a string.
   * @return True if the node holds a string, false otherwise.
   */
  [[nodiscard]] bool is_string() const { return std::holds_alternative<std::string>(value); }

  /**
   * @brief Checks if the node holds an integer.
   * @return True if the node holds an integer, false otherwise.
   */
  [[nodiscard]] bool is_integer() const { return std::holds_alternative<int64_t>(value); }

  /**
   * @brief Checks if the node holds a float.
   * @return True if the node holds a float, false otherwise.
   */
  [[nodiscard]] bool is_float() const { return std::holds_alternative<double>(value); }

  /**
   * @brief Checks if the node holds a boolean.
   * @return True if the node holds a boolean, false otherwise.
   */
  [[nodiscard]] bool is_boolean() const { return std::holds_alternative<bool>(value); }

  /**
   * @brief Checks if the node holds an array.
   * @return True if the node holds an array, false otherwise.
   */
  [[nodiscard]] bool is_array() const { return std::holds_alternative<std::vector<std::shared_ptr<Node> > >(value); }

  /**
   * @brief Checks if the node holds a table.
   * @return True if the node holds a table, false otherwise.
   */
  [[nodiscard]] bool is_table() const {
    return std::holds_alternative<std::map<std::string, std::shared_ptr<Node> > >(value);
  }

  // Const value retrieval methods with type safety

  /**
   * @brief Gets the string value of the node.
   * @return A const reference to the string value.
   * @throws std::bad_variant_access if the node does not hold a string.
   */
  [[nodiscard]] const std::string &as_string() const { return std::get<std::string>(value); }

  /**
   * @brief Gets the integer value of the node.
   * @return The integer value.
   * @throws std::bad_variant_access if the node does not hold an integer.
   */
  [[nodiscard]] int64_t as_integer() const { return std::get<int64_t>(value); }

  /**
   * @brief Gets the float value of the node.
   * @return The float value.
   * @throws std::bad_variant_access if the node does not hold a float.
   */
  [[nodiscard]] double as_float() const { return std::get<double>(value); }

  /**
   * @brief Gets the boolean value of the node.
   * @return The boolean value.
   * @throws std::bad_variant_access if the node does not hold a boolean.
   */
  [[nodiscard]] bool as_boolean() const { return std::get<bool>(value); }

  /**
   * @brief Gets the array value of the node.
   * @return A const reference to the vector of nodes.
   * @throws std::bad_variant_access if the node does not hold an array.
   */
  [[nodiscard]] const std::vector<std::shared_ptr<Node> > &as_array() const {
    return std::get<std::vector<std::shared_ptr<Node> > >(value);
  }

  /**
   * @brief Gets the table value of the node.
   * @return A const reference to the map of nodes.
   * @throws std::bad_variant_access if the node does not hold a table.
   */
  [[nodiscard]] const std::map<std::string, std::shared_ptr<Node> > &as_table() const {
    return std::get<std::map<std::string, std::shared_ptr<Node> > >(value);
  }

  // Mutable value retrieval methods

  /**
   * @brief Gets the string value of the node.
   * @return A reference to the string value.
   * @throws std::bad_variant_access if the node does not hold a string.
   */
  std::string &as_string() { return std::get<std::string>(value); }

  /**
   * @brief Gets the integer value of the node.
   * @return A reference to the integer value.
   * @throws std::bad_variant_access if the node does not hold an integer.
   */
  int64_t &as_integer() { return std::get<int64_t>(value); }

  /**
   * @brief Gets the float value of the node.
   * @return A reference to the float value.
   * @throws std::bad_variant_access if the node does not hold a float.
   */
  double &as_float() { return std::get<double>(value); }

  /**
   * @brief Gets the boolean value of the node.
   * @return A reference to the boolean value.
   * @throws std::bad_variant_access if the node does not hold a boolean.
   */
  bool &as_boolean() { return std::get<bool>(value); }

  /**
   * @brief Gets the array value of the node.
   * @return A reference to the vector of nodes.
   * @throws std::bad_variant_access if the node does not hold an array.
   */
  std::vector<std::shared_ptr<Node> > &as_array() {
    return std::get<std::vector<std::shared_ptr<Node> > >(value);
  }

  /**
   * @brief Gets the table value of the node.
   * @return A reference to the map of nodes.
   * @throws std::bad_variant_access if the node does not hold a table.
   */
  std::map<std::string, std::shared_ptr<Node> > &as_table() {
    return std::get<std::map<std::string, std::shared_ptr<Node> > >(value);
  }

  // Templated accessors

  /**
   * @brief Generic method to get the value of the node.
   * @tparam T The type of the value to retrieve.
   * @return A const reference to the value.
   * @throws std::bad_variant_access if the node does not hold the specified type.
   */
  template<typename T>
  const T &as() const {
    return std::get<T>(value);
  }

  /**
   * @brief Generic method to get the value of the node.
   * @tparam T The type of the value to retrieve.
   * @return A reference to the value.
   * @throws std::bad_variant_access if the node does not hold the specified type.
   */
  template<typename T>
  T &as() {
    return std::get<T>(value);
  }
};

inline void PrintNode(const Node &node, const int indent = 0) {
  const std::string indentation(indent * 2, ' ');
  if (node.is_string()) {
    std::cout << '"' << node.as_string() << '"';
  } else if (node.is_integer()) {
    std::cout << node.as_integer();
  } else if (node.is_float()) {
    std::cout << node.as_float();
  } else if (node.is_boolean()) {
    std::cout << (node.as_boolean() ? "true" : "false");
  } else if (node.is_table()) {
    std::cout << "{\n";
    const auto &table = node.as_table();
    for (auto it = table.begin(); it != table.end(); ++it) {
      std::cout << indentation << "  \"" << it->first << "\": ";
      PrintNode(*(it->second), indent + 1);
      if (std::next(it) != table.end()) {
        std::cout << ",";
      }
      std::cout << "\n";
    }
    std::cout << indentation << "}";
  }
}
}

#endif //TOML_CPP_NODE_H
