#ifndef LOCALTIME_H
#define LOCALTIME_H
#include <charconv>
#include <cstdint>
#include <iomanip>
#include <iosfwd>
#include <optional>

#include "toml_cpp/util.h"

namespace toml {
/**
 * @brief A class to store a specific time of day.
 *
 * This class represents a time of day (hour, minute, second, nanosecond)
 * without any date or time zone information.
 */
struct LocalTime {
  uint8_t hour{0};         ///< The hour component, from 0 to 23.
  uint8_t minute{0};       ///< The minute component, from 0 to 59.
  uint8_t second{0};       ///< The second component, from 0 to 59.
  uint32_t nanosecond{0};  ///< The fractional nanoseconds component, from 0 to 999,999,999.

  LocalTime() = default;

  /**
   * @brief Constructs a LocalTime from various integer types.
   * @details This constructor is a template to allow construction from any
   * integral type (e.g. int, long, uint8_t). It uses static_assert
   * to ensure that floating-point types are not used, preventing
   * silent truncation and data loss.
   *
   * @tparam H  Type for hour, must be an integral type.
   * @tparam M  Type for minute, must be an integral type.
   * @tparam S  Type for second, must be an integral type.
   * @tparam NS Type for nanosecond, must be an integral type.
   *
   * @param h The hour component (0-23).
   * @param m The minute component (0-59).
   * @param s The second component (0-59).
   * @param ns The nanosecond component (0-999,999,999).
   */
  template <typename H, typename M, typename S, typename NS = int>
  constexpr explicit LocalTime(H h, M m, S s, NS ns = 0) noexcept
      : hour(static_cast<uint8_t>(h)),
        minute(static_cast<uint8_t>(m)),
        second(static_cast<uint8_t>(s)),
        nanosecond(static_cast<uint32_t>(ns)) {
    static_assert(std::is_integral_v<H>, "Hour type must be an integral type.");
    static_assert(std::is_integral_v<M>, "Minute type must be an integral type.");
    static_assert(std::is_integral_v<S>, "Second type must be an integral type.");
    static_assert(std::is_integral_v<NS>, "Nanosecond type must be an integral type.");
  }

 private:
  static constexpr uint64_t Pack(const LocalTime &lt) noexcept {
    return static_cast<uint64_t>(lt.hour) << 48 | static_cast<uint64_t>(lt.minute) << 40 |
           static_cast<uint64_t>(lt.second) << 32 | static_cast<uint64_t>(lt.nanosecond);
  }

 public:
  /**
   * @brief Equality operator.
   */
  friend constexpr bool operator==(const LocalTime &lhs, const LocalTime &rhs) noexcept {
    return lhs.hour == rhs.hour && lhs.minute == rhs.minute && lhs.second == rhs.second &&
           lhs.nanosecond == rhs.nanosecond;
  }

  /**
   * @brief Inequality operator.
   */
  friend constexpr bool operator!=(const LocalTime &lhs, const LocalTime &rhs) noexcept {
    return !(lhs == rhs);
  }

  /**
   * @brief Less-than operator.
   */
  friend constexpr bool operator<(const LocalTime &lhs, const LocalTime &rhs) noexcept {
    return Pack(lhs) < Pack(rhs);
  }

  /**
   * @brief Less-than-or-equal-to operator.
   */
  friend constexpr bool operator<=(const LocalTime &lhs, const LocalTime &rhs) noexcept {
    return Pack(lhs) <= Pack(rhs);
  }

  /**
   * @brief Greater-than operator.
   */
  friend constexpr bool operator>(const LocalTime &lhs, const LocalTime &rhs) noexcept {
    return Pack(lhs) > Pack(rhs);
  }

  /**
   * @brief Greater-than-or-equal-to operator.
   */
  friend constexpr bool operator>=(const LocalTime &lhs, const LocalTime &rhs) noexcept {
    return Pack(lhs) >= Pack(rhs);
  }

  /**
   * @brief Prints a time to a stream as HH:MM:SS.nnnnnnnnn (per RFC 3339).
   */
  friend std::ostream &operator<<(std::ostream &lhs, const LocalTime &rhs) {
    const std::ios_base::fmtflags flags(lhs.flags());
    const char fill(lhs.fill());

    // Cast uint8_t to int to prevent it from being interpreted as a character.
    lhs << std::setfill('0') << std::setw(2) << static_cast<int>(rhs.hour) << ":"
        << std::setfill('0') << std::setw(2) << static_cast<int>(rhs.minute) << ":"
        << std::setfill('0') << std::setw(2) << static_cast<int>(rhs.second) << "."
        << std::setfill('0') << std::setw(9) << rhs.nanosecond;

    lhs.flags(flags);
    lhs.fill(fill);

    return lhs;
  }

  /**
   * @brief Parses a time string in the format HH:MM:SS[.nnnnnnnnn].
   * @param sv The string_view to parse.
   * @return An optional containing the LocalTime on success, or an empty
   * optional on failure.
   */
  static std::optional<LocalTime> FromString(const std::string_view sv) noexcept {
    static constexpr size_t HOUR_POS = 0;
    static constexpr size_t MIN_POS = 3;
    static constexpr size_t SEC_POS = 6;
    static constexpr size_t TIME_LEN = 2;     // Length of HH, MM, SS components
    static constexpr size_t MINIMUM_LEN = 8;  // Minimum length for "HH:MM:SS"
    static constexpr size_t MAX_FRACTIONAL_DIGITS = 9;

    if (sv.length() < MINIMUM_LEN || sv[2] != ':' || sv[5] != ':') {
      return std::nullopt;
    }

    uint8_t h = 0, m = 0, s = 0;
    uint32_t ns = 0;

    const char *const data = sv.data();
    const char *const end = data + sv.length();

    if (!util::parse_component(data, HOUR_POS, TIME_LEN, h) ||
        !util::parse_component(data, MIN_POS, TIME_LEN, m) ||
        !util::parse_component(data, SEC_POS, TIME_LEN, s)) {
      return std::nullopt;
    }

    const char *current = data + MINIMUM_LEN;

    // Optional: Parse fractional seconds .nnnnnnnnn
    if (current != end && *current == '.') {
      ++current;  // skip '.'

      // If nothing remains after the '.', it's invalid
      if (current == end) {
        return std::nullopt;
      }

      const size_t fractional_start = current - data;
      const size_t available_digits = end - current;
      const size_t digits_to_parse = std::min(MAX_FRACTIONAL_DIGITS, available_digits);

      if (!util::parse_component(data, fractional_start, digits_to_parse, ns)) {
        return std::nullopt;
      }

      for (size_t i = digits_to_parse; i < MAX_FRACTIONAL_DIGITS; ++i) {
        ns *= 10;
      }

      current = data + fractional_start + available_digits;  // Advance current to the consumed end
    }

    // Must consume entire input
    if (current != end) {
      return std::nullopt;
    }

    // Validate component ranges
    if (h > 23 || m > 59 || s > 60) {
      return std::nullopt;
    }

    return LocalTime(h, m, s, ns);
  }
};
}  // namespace toml

#endif  // LOCALTIME_H
