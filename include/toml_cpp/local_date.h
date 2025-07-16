#ifndef TOML_CPP_LOCALDATE_H
#define TOML_CPP_LOCALDATE_H
#include <charconv>
#include <cstdint>
#include <iomanip>
#include <iosfwd>
#include <optional>

#include "toml_cpp/util.h"

namespace toml {
/**
 * @brief A class to store a specific calendar date.
 *
 * This class represents a date (year, month, day) without any
 * time-of-day or time zone information.
 */
struct LocalDate {
  uint16_t year{0};  ///< The year component.
  uint8_t month{0};  ///< The month component, from 1 to 12.
  uint8_t day{0};    ///< The day component, from 1 to 31.

  LocalDate() = default;

  /**
   * @brief Constructs a LocalDate from various integer types.
   * @tparam Y Type for year, must be an integral type.
   * @tparam M Type for month, must be an integral type.
   * @tparam D Type for day, must be an integral type.
   * @param y The year component.
   * @param m The month component (1-12).
   * @param d The day component (1-31).
   */
  template <typename Y, typename M, typename D>
  constexpr explicit LocalDate(Y y, M m, D d) noexcept
      : year(static_cast<uint16_t>(y)),
        month(static_cast<uint8_t>(m)),
        day(static_cast<uint8_t>(d)) {
    static_assert(std::is_integral_v<Y>, "Year type must be an integral type.");
    static_assert(std::is_integral_v<M>, "Month type must be an integral type.");
    static_assert(std::is_integral_v<D>, "Day type must be an integral type.");
  }

 private:
  /**
   * @brief Packs the date components into a single 32-bit integer for efficient
   * comparison.
   *
   * The layout is designed so that the natural integer comparison of packed
   * values matches the chronological order of the dates. The packed integer is
   * an implementation detail used by the comparison operators.
   *
   * Format (32-bit integer, from high bits to low bits):
   * - 16 bits for the year
   * - 4 bits for the month
   * - 5 bits for the day
   *
   * @return A 32-bit unsigned integer representing the packed date.
   */
  static constexpr uint32_t Pack(const LocalDate &date) noexcept {
    return static_cast<uint32_t>(date.year) << 9 | static_cast<uint32_t>(date.month) << 5 |
           static_cast<uint32_t>(date.day);
  }

 public:
  /**
   * @brief Equality operator.
   */
  friend constexpr bool operator==(const LocalDate &lhs, const LocalDate &rhs) noexcept {
    return lhs.year == rhs.year && lhs.month == rhs.month && lhs.day == rhs.day;
  }

  /**
   * @brief Inequality operator.
   */
  friend constexpr bool operator!=(const LocalDate &lhs, const LocalDate &rhs) noexcept {
    return !(lhs == rhs);
  }

  /**
   * @brief Less-than operator.
   */
  friend constexpr bool operator<(const LocalDate &lhs, const LocalDate &rhs) noexcept {
    return Pack(lhs) < Pack(rhs);
  }

  /**
   * @brief Less-than-or-equal-to operator.
   */
  friend constexpr bool operator<=(const LocalDate &lhs, const LocalDate &rhs) noexcept {
    return Pack(lhs) <= Pack(rhs);
  }

  /**
   * @brief Greater-than operator.
   */
  friend constexpr bool operator>(const LocalDate &lhs, const LocalDate &rhs) noexcept {
    return Pack(lhs) > Pack(rhs);
  }

  /**
   * @brief Greater-than-or-equal-to operator.
   */
  friend constexpr bool operator>=(const LocalDate &lhs, const LocalDate &rhs) noexcept {
    return Pack(lhs) >= Pack(rhs);
  }

  /**
   * @brief Prints a date to a stream as YYYY-MM-DD.
   */
  friend std::ostream &operator<<(std::ostream &lhs, const LocalDate &rhs) {
    const std::ios_base::fmtflags flags(lhs.flags());
    const char fill(lhs.fill());

    lhs << std::setfill('0') << std::setw(4) << static_cast<int>(rhs.year) << "-"
        << std::setfill('0') << std::setw(2) << static_cast<int>(rhs.month) << "-"
        << std::setfill('0') << std::setw(2) << static_cast<int>(rhs.day);

    lhs.flags(flags);
    lhs.fill(fill);

    return lhs;
  }

  /**
   * @brief Parses a date string in the format YYYY-MM-DD.
   * @param sv The string_view to parse.
   * @return An optional containing the LocalDate on success, or an empty
   * optional on failure.
   */
  static std::optional<LocalDate> FromString(const std::string_view sv) noexcept {
    constexpr size_t YEAR_POS = 0;
    constexpr size_t YEAR_LEN = 4;
    constexpr size_t MONTH_POS = 5;
    constexpr size_t MONTH_LEN = 2;
    constexpr size_t DAY_POS = 8;
    constexpr size_t DAY_LEN = 2;
    constexpr size_t TOTAL_LEN = 10;

    if (sv.length() != TOTAL_LEN || sv[4] != '-' || sv[7] != '-') {
      return std::nullopt;
    }

    uint16_t y = 0;
    uint8_t m = 0, d = 0;

    const char *const data = sv.data();

    if (!util::parse_component(data, YEAR_POS, YEAR_LEN, y) ||
        !util::parse_component(data, MONTH_POS, MONTH_LEN, m) ||
        !util::parse_component(data, DAY_POS, DAY_LEN, d)) {
      return std::nullopt;
    }

    // Validate component ranges
    if (m < 1 || m > 12 || d < 1 || d > 31) {
      return std::nullopt;
    }

    return LocalDate(y, m, d);
  }
};
}  // namespace toml

#endif  // TOML_CPP_LOCALDATE_H
