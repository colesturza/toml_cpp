#ifndef TOML_CPP_PARSER_HPP
#define TOML_CPP_PARSER_HPP
#include <vector>

#include "token.hpp"
#include "toml_cpp/node.hpp"

namespace toml {
class parser {
public:
  explicit parser(std::vector<token> tokens);

  Node Parse();

private:
  size_t pos_ = 0;
  std::vector<token> tokens_;
};
}

#endif //TOML_CPP_PARSER_HPP
