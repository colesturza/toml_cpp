//
// Created by coles on 7/11/2025.
//

#ifndef TOML_CPP_UNICODE_HPP
#define TOML_CPP_UNICODE_HPP

#include <string>
#include <stdexcept>
#include <iomanip>
#include <cstdint>

namespace toml::util {
inline std::string unicode_codepoint(const std::string &hex, int length) {
  // Validate input length (4 or 8)
  if ((length != 4 && length != 8) || hex.length() != static_cast<size_t>(length)) {
    throw std::runtime_error("Invalid Unicode escape sequence length");
  }

  // Convert hex to integer
  uint32_t codepoint = 0;
  try {
    codepoint = static_cast<uint32_t>(std::stoul(hex, nullptr, 16));
  } catch (...) {
    throw std::runtime_error("Invalid hex in Unicode escape");
  }

  // UTF-8 encoding
  std::string utf8;
  if (codepoint <= 0x7F) {
    utf8 += static_cast<char>(codepoint);
  } else if (codepoint <= 0x7FF) {
    utf8 += static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F));
    utf8 += static_cast<char>(0x80 | (codepoint & 0x3F));
  } else if (codepoint <= 0xFFFF) {
    utf8 += static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F));
    utf8 += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
    utf8 += static_cast<char>(0x80 | (codepoint & 0x3F));
  } else if (codepoint <= 0x10FFFF) {
    utf8 += static_cast<char>(0xF0 | ((codepoint >> 18) & 0x07));
    utf8 += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
    utf8 += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
    utf8 += static_cast<char>(0x80 | (codepoint & 0x3F));
  } else {
    throw std::runtime_error("Unicode codepoint out of range");
  }

  return utf8;
}
}

#endif //TOML_CPP_UNICODE_HPP
