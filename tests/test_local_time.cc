#include <gtest/gtest.h>
#include <sstream>
#include <string>

#include "toml_cpp/local_time.h"

class LocalTimeTest : public ::testing::Test {};

TEST(LocalTimeTest, Construction) {
  // Default construction
  constexpr toml::LocalTime t1;
  EXPECT_EQ(t1.hour, 0);
  EXPECT_EQ(t1.minute, 0);
  EXPECT_EQ(t1.second, 0);
  EXPECT_EQ(t1.nanosecond, 0);

  // Value construction
  constexpr toml::LocalTime t2(13, 30, 5, 123456789);
  EXPECT_EQ(t2.hour, 13);
  EXPECT_EQ(t2.minute, 30);
  EXPECT_EQ(t2.second, 5);
  EXPECT_EQ(t2.nanosecond, 123456789);

  // Construction without nanoseconds
  constexpr toml::LocalTime t3(23, 59, 59);
  EXPECT_EQ(t3.hour, 23);
  EXPECT_EQ(t3.minute, 59);
  EXPECT_EQ(t3.second, 59);
  EXPECT_EQ(t3.nanosecond, 0);
}

TEST(LocalTimeTest, Comparison) {
  constexpr toml::LocalTime t1(10, 20, 30, 40);
  constexpr toml::LocalTime t2(10, 20, 30, 40);
  constexpr toml::LocalTime t3(11, 0, 0, 0);     // Greater hour
  constexpr toml::LocalTime t4(10, 21, 0, 0);    // Greater minute
  constexpr toml::LocalTime t5(10, 20, 31, 0);   // Greater second
  constexpr toml::LocalTime t6(10, 20, 30, 41);  // Greater nanosecond

  EXPECT_EQ(t1, t2);
  EXPECT_NE(t1, t3);

  EXPECT_LT(t1, t3);
  EXPECT_LT(t1, t4);
  EXPECT_LT(t1, t5);
  EXPECT_LT(t1, t6);

  EXPECT_GT(t3, t1);
  EXPECT_GT(t4, t1);
  EXPECT_GT(t5, t1);
  EXPECT_GT(t6, t1);

  EXPECT_LE(t1, t2);
  EXPECT_LE(t1, t3);

  EXPECT_GE(t2, t1);
  EXPECT_GE(t3, t1);
}

TEST(LocalTimeTest, FromStringValid) {
  auto r1 = toml::LocalTime::FromString("01:02:03.123456789");
  EXPECT_TRUE(r1.has_value());
  EXPECT_EQ(*r1, toml::LocalTime(1, 2, 3, 123456789));

  auto r2 = toml::LocalTime::FromString("23:59:59");
  EXPECT_TRUE(r2.has_value());
  EXPECT_EQ(*r2, toml::LocalTime(23, 59, 59, 0));

  auto r3 = toml::LocalTime::FromString("08:10:20.123");  // Shorter fraction
  EXPECT_TRUE(r3.has_value());
  EXPECT_EQ(*r3, toml::LocalTime(8, 10, 20, 123000000));

  auto r4 = toml::LocalTime::FromString("09:15:25.123456789123");  // Longer fraction (truncates)
  EXPECT_TRUE(r4.has_value());
  EXPECT_EQ(*r4, toml::LocalTime(9, 15, 25, 123456789));

  auto r5 = toml::LocalTime::FromString("00:00:00.0");
  EXPECT_TRUE(r5.has_value());
  EXPECT_EQ(*r5, toml::LocalTime(0, 0, 0, 0));
}

TEST(LocalTimeTest, FromStringInvalid) {
  EXPECT_FALSE(toml::LocalTime::FromString("24:00:00").has_value());        // Bad hour
  EXPECT_FALSE(toml::LocalTime::FromString("0a:00:00").has_value());        // Non-digit hour
  EXPECT_FALSE(toml::LocalTime::FromString("00:0a:00").has_value());        // Non-digit minute
  EXPECT_FALSE(toml::LocalTime::FromString("00:00:0a").has_value());        // Non-digit second
  EXPECT_FALSE(toml::LocalTime::FromString("00:60:00").has_value());        // Bad minute
  EXPECT_FALSE(toml::LocalTime::FromString("00:00:60").has_value());        // Bad second
  EXPECT_FALSE(toml::LocalTime::FromString("1:2:3").has_value());           // Not zero-padded
  EXPECT_FALSE(toml::LocalTime::FromString("01:02:03.").has_value());       // Dangling decimal
  EXPECT_FALSE(toml::LocalTime::FromString("01:02:03.a").has_value());      // Non-digit fraction
  EXPECT_FALSE(toml::LocalTime::FromString("01-02-03").has_value());        // Wrong separator
  EXPECT_FALSE(toml::LocalTime::FromString("01:02:03 extra").has_value());  // Extra text
  EXPECT_FALSE(toml::LocalTime::FromString("").has_value());                // Empty string
  EXPECT_FALSE(toml::LocalTime::FromString("01:2:03").has_value());         // Not zero-padded
}

TEST(LocalTimeTest, Streaming) {
  constexpr toml::LocalTime t1(7, 8, 9, 123456789);
  std::stringstream ss;
  ss << t1;
  EXPECT_EQ(ss.str(), "07:08:09.123456789");

  constexpr toml::LocalTime t2(23, 59, 59);
  ss.str("");  // Clear the stream
  ss << t2;
  EXPECT_EQ(ss.str(), "23:59:59.000000000");

  constexpr toml::LocalTime t3(1, 2, 3, 456000000);
  ss.str("");  // Clear the stream
  ss << t3;
  EXPECT_EQ(ss.str(), "01:02:03.456000000");
}
