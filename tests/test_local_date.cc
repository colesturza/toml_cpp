#include <gtest/gtest.h>
#include <sstream>

#include "toml_cpp/local_date.h"

class LocalDateTest : public ::testing::Test {};

TEST_F(LocalDateTest, FromStringValid) {
  const auto r = toml::LocalDate::FromString("2023-07-25");
  EXPECT_TRUE(r.has_value());
  EXPECT_EQ(r->year, 2023);
  EXPECT_EQ(r->month, 7);
  EXPECT_EQ(r->day, 25);
}

TEST_F(LocalDateTest, FromStringInvalidLength) {
  EXPECT_FALSE(toml::LocalDate::FromString("2023-07-2").has_value());
  EXPECT_FALSE(toml::LocalDate::FromString("2023-07-255").has_value());
  EXPECT_FALSE(toml::LocalDate::FromString("2023-7-25").has_value());
}

TEST_F(LocalDateTest, FromStringInvalidSeparators) {
  EXPECT_FALSE(toml::LocalDate::FromString("2023/07/25").has_value());
  EXPECT_FALSE(toml::LocalDate::FromString("2023-07/25").has_value());
  EXPECT_FALSE(toml::LocalDate::FromString("2023 07 25").has_value());
}

TEST_F(LocalDateTest, FromStringNonDigitChars) {
  EXPECT_FALSE(toml::LocalDate::FromString("202a-07-25").has_value());
  EXPECT_FALSE(toml::LocalDate::FromString("2023-0b-25").has_value());
  EXPECT_FALSE(toml::LocalDate::FromString("2023-07-2c").has_value());
}

TEST_F(LocalDateTest, FromStringOutOfRange) {
  EXPECT_FALSE(toml::LocalDate::FromString("2023-13-01").has_value());  // Invalid month
  EXPECT_FALSE(toml::LocalDate::FromString("2023-00-01").has_value());  // Invalid month
  EXPECT_FALSE(toml::LocalDate::FromString("2023-01-32").has_value());  // Invalid day
  EXPECT_FALSE(toml::LocalDate::FromString("2023-01-00").has_value());  // Invalid day
}

TEST_F(LocalDateTest, Equality) {
  constexpr toml::LocalDate d1(2023, 7, 25);
  constexpr toml::LocalDate d2(2023, 7, 25);
  constexpr toml::LocalDate d3(2024, 7, 25);
  constexpr toml::LocalDate d4(2023, 8, 25);
  constexpr toml::LocalDate d5(2023, 7, 26);

  EXPECT_TRUE(d1 == d2);
  EXPECT_FALSE(d1 == d3);
  EXPECT_FALSE(d1 == d4);
  EXPECT_FALSE(d1 == d5);

  EXPECT_FALSE(d1 != d2);
  EXPECT_TRUE(d1 != d3);
}

TEST_F(LocalDateTest, LessThan) {
  constexpr toml::LocalDate d1(2023, 7, 25);

  // Compare by year
  constexpr toml::LocalDate d_year(2024, 1, 1);
  EXPECT_TRUE(d1 < d_year);
  EXPECT_FALSE(d_year < d1);

  // Compare by month
  constexpr toml::LocalDate d_month(2023, 8, 1);
  EXPECT_TRUE(d1 < d_month);
  EXPECT_FALSE(d_month < d1);

  // Compare by day
  constexpr toml::LocalDate d_day(2023, 7, 26);
  EXPECT_TRUE(d1 < d_day);
  EXPECT_FALSE(d_day < d1);

  // Equality case
  constexpr toml::LocalDate d_equal(2023, 7, 25);
  EXPECT_FALSE(d1 < d_equal);
}

TEST_F(LocalDateTest, GreaterThan) {
  constexpr toml::LocalDate d1(2023, 7, 25);
  constexpr toml::LocalDate d2(2022, 12, 31);
  EXPECT_TRUE(d1 > d2);
  EXPECT_FALSE(d2 > d1);
}

TEST_F(LocalDateTest, LessThanOrEqual) {
  constexpr toml::LocalDate d1(2023, 7, 25);
  constexpr toml::LocalDate d2(2023, 7, 25);
  constexpr toml::LocalDate d3(2023, 7, 26);
  EXPECT_TRUE(d1 <= d2);
  EXPECT_TRUE(d1 <= d3);
  EXPECT_FALSE(d3 <= d1);
}

TEST_F(LocalDateTest, GreaterThanOrEqual) {
  constexpr toml::LocalDate d1(2023, 7, 25);
  constexpr toml::LocalDate d2(2023, 7, 25);
  constexpr toml::LocalDate d3(2023, 7, 24);
  EXPECT_TRUE(d1 >= d2);
  EXPECT_TRUE(d1 >= d3);
  EXPECT_FALSE(d3 >= d1);
}

TEST_F(LocalDateTest, Ostream) {
  constexpr toml::LocalDate d1(2023, 7, 25);
  std::stringstream ss1;
  ss1 << d1;
  EXPECT_EQ(ss1.str(), "2023-07-25");

  // Test zero-padding for month and day
  constexpr toml::LocalDate d2(2024, 1, 5);
  std::stringstream ss2;
  ss2 << d2;
  EXPECT_EQ(ss2.str(), "2024-01-05");
}

TEST_F(LocalDateTest, DefaultConstructor) {
  constexpr toml::LocalDate d;
  EXPECT_EQ(d.year, 0);
  EXPECT_EQ(d.month, 0);
  EXPECT_EQ(d.day, 0);
}

TEST_F(LocalDateTest, MemberConstructor) {
  constexpr toml::LocalDate d(2025, 12, 31);
  EXPECT_EQ(d.year, 2025);
  EXPECT_EQ(d.month, 12);
  EXPECT_EQ(d.day, 31);
}
