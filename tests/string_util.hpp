#ifndef TOML_CPP_TESTS_STRING_UTIL_HPP
#define TOML_CPP_TESTS_STRING_UTIL_HPP

#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

namespace toml::tests::util {
inline std::string dedent(const std::string &input) {
  std::istringstream iss(input);
  std::string line;
  size_t min_indent = std::string::npos;

  // Find minimum indentation (skip empty lines)
  while (std::getline(iss, line)) {
    if (line.empty()) continue;
    size_t indent = 0;
    while (indent < line.size() && std::isspace(static_cast<unsigned char>(line[indent]))) {
      ++indent;
    }
    if (indent < line.size()) {
      // non-blank line
      min_indent = std::min(min_indent, indent);
    }
  }

  if (min_indent == std::string::npos) {
    // string is empty or all blank lines
    return input;
  }

  // Reset stream to beginning
  iss.clear();
  iss.seekg(0);

  std::ostringstream oss;
  while (std::getline(iss, line)) {
    if (line.size() >= min_indent) {
      oss << line.substr(min_indent);
    } else {
      oss << line; // line shorter than indent (likely blank)
    }
    oss << '\n';
  }
  return oss.str();
}
}

#endif //TOML_CPP_TESTS_STRING_UTIL_HPP
