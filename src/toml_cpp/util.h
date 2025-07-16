#ifndef TOML_CPP_UTIL_H
#define TOML_CPP_UTIL_H

#include <charconv>

namespace toml::util {
/**
 * @brief A utility to parse an integer component from a string.
 * @tparam T The integer type to parse.
 * @param data The start of the string data.
 * @param pos The starting position of the component.
 * @param len The length of the component.
 * @param value A reference to store the parsed value.
 * @return True on success, false on failure.
 */
template <typename T>
static bool parse_component(const char *data,
                            const size_t pos,
                            const size_t len,
                            T &value) noexcept {
  static_assert(std::is_integral_v<T>, "value type must be an integral type.");
  const char *start = data + pos;
  const char *end = start + len;
  auto result = std::from_chars(start, end, value);
  // Parsing is successful only if there's no error AND all characters were
  // consumed.
  return result.ec == std::errc() && result.ptr == end;
}
}  // namespace toml::util

#endif  // TOML_CPP_UTIL_H
