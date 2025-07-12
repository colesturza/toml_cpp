#include <fstream>
#include <iostream>
#include <sstream>
#include <gtest/gtest.h>

#include "token.hpp"
#include "lexer.hpp"
#include "string_util.hpp"

std::string read_file(const std::string &file_path) {
  std::ifstream file(file_path);
  if (!file.is_open()) {
    std::cerr << "Failed to open file " << file_path << std::endl;
    exit(1);
  }
  std::stringstream ss;
  ss << file.rdbuf();
  return ss.str();
}

TEST(LexerTest, TokenizesExampleFileCorrectly) {
  const std::string input = R"(
    key = "value"
    bare_key = "value"
    bare-key = "value"
    1234 = "value"

    "127.0.0.1" = "value"
    "character encoding" = "value"
    "ʎǝʞ" = "value"
    'key2' = "value"
    'quoted "value"' = "value"

    "" = "blank"     # VALID but discouraged

    name = "Orange"
    physical.color = "orange"
    physical.shape = "round"
    site."google.com" = true

    int1 = +99
    int2 = 42
    int3 = 0
    int4 = -17

    int5 = 1_000
    int6 = 5_349_221
    int7 = 53_49_221  # Indian number system grouping
    int8 = 1_2_3_4_5  # VALID but discouraged

    # hexadecimal with prefix `0x`
    hex1 = 0xDEADBEEF
    hex2 = 0xdeadbeef
    hex3 = 0xdead_beef

    # octal with prefix `0o`
    oct1 = 0o01234567
    oct2 = 0o755 # useful for Unix file permissions

    # binary with prefix `0b`
    bin1 = 0b11010110

    # fractional
    flt1 = +1.0
    flt2 = 3.1415
    flt3 = -0.01

    # exponent
    flt4 = 5e+22
    flt5 = 1e06
    flt6 = -2E-2

    # both
    flt7 = 6.626e-34

    flt8 = 224_617.445_991_228

    # infinity
    sf1 = inf  # positive infinity
    sf2 = +inf # positive infinity
    sf3 = -inf # negative infinity

    # not a number
    sf4 = nan  # actual sNaN/qNaN encoding is implementation-specific
    sf5 = +nan # same as `nan`
    sf6 = -nan # valid, actual encoding is implementation-specific

    bool1 = true
    bool2 = false

    # The following three strings are byte-for-byte equivalent:
    str1 = "The quick brown fox jumps over the lazy dog."

    str2 = """
    The quick brown \


      fox jumps over \
        the lazy dog."""

    str3 = """\
           The quick brown \
           fox jumps over \
           the lazy dog.\
           """

    str4 = """Here are two quotation marks: "". Simple enough."""
    # str5 = """Here are three quotation marks: """."""  # INVALID
    str5 = """Here are three quotation marks: ""\"."""
    str6 = """Here are fifteen quotation marks: ""\"""\"""\"""\"""\"."""

    winpath  = 'C:\Users\nodejs\templates'
    winpath2 = '\\ServerX\admin$\system32\'
    quoted   = 'Tom "Dubs" Preston-Werner'
    regex    = '<\i\c*\s*>'

    regex2 = '''I [dw]on't need \d{2} apples'''
    lines  = '''
    The first newline is
    trimmed in raw strings.
       All other whitespace
       is preserved.
    '''

    quot15 = '''Here are fifteen quotation marks: """""""""""""""'''

    # apos15 = '''Here are fifteen apostrophes: ''''''''''''''''''  # INVALID
    apos15 = "Here are fifteen apostrophes: '''''''''''''''"

    odt1 = 1979-05-27T07:32:00Z
    odt2 = 1979-05-27T00:32:00-07:00
    odt3 = 1979-05-27T00:32:00.999999-07:00

    odt4 = 1979-05-27 07:32:00Z

    ldt1 = 1979-05-27T07:32:00
    ldt2 = 1979-05-27T00:32:00.999999

    ld1 = 1979-05-27

    lt1 = 07:32:00
    lt2 = 00:32:00.999999

    integers = [ 1, 2, 3 ]
    colors = [ "red", "yellow", "green" ]
    nested_arrays_of_ints = [ [ 1, 2 ], [3, 4, 5] ]
    nested_mixed_array = [ [ 1, 2 ], ["a", "b", "c"] ]
    string_array = [ "all", 'strings', """are the same""", '''type''' ]

    # Mixed-type arrays are allowed
    numbers = [ 0.1, 0.2, 0.5, 1, 2, 5 ]

    integers2 = [
      1, 2, 3
    ]

    integers3 = [
      1,
      2, # this is ok
    ]

    [table]

    [table-1]
    key1 = "some string"
    key2 = 123

    [table-2]
    key1 = "another string"
    key2 = 456

    [dog."tater.man"]
    type.name = "pug"

    [[products]]
    name = "Hammer"
    sku = 738594937

    [[products]]  # empty table within the array

    [[products]]
    name = "Nail"
    sku = 284758393

    color = "gray"

    [[fruits]]
    name = "apple"

    [fruits.physical]  # subtable
    color = "red"
    shape = "round"

    [[fruits.varieties]]  # nested array of tables
    name = "red delicious"

    [[fruits.varieties]]
    name = "granny smith"


    [[fruits]]
    name = "banana"

    [[fruits.varieties]]
    name = "plantain"

    name = { first = "Tom", last = "Preston-Werner" }
    point = { x = 1, y = 2 }
    animal = { type.name = "pug" }

    points = [ { x = 1, y = 2, z = 3 },
           { x = 7, y = 8, z = 9 },
           { x = 2, y = 4, z = 8 } ]
  )";

  const std::string dedented_input = toml::tests::util::dedent(input);

  const std::vector expected_tokens = {
    {toml::TokenType::TOKEN_KEY, "key"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "value"},
    {toml::TokenType::TOKEN_KEY, "bare_key"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "value"},
    {toml::TokenType::TOKEN_KEY, "bare-key"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "value"},
    {toml::TokenType::TOKEN_KEY, "1234"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "value"},
    {toml::TokenType::TOKEN_KEY, "127.0.0.1"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "value"},
    {toml::TokenType::TOKEN_KEY, "character encoding"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "value"},
    {toml::TokenType::TOKEN_KEY, "ʎǝʞ"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "value"},
    {toml::TokenType::TOKEN_KEY, "key2"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "value"},
    {toml::TokenType::TOKEN_KEY, "quoted \"value\""},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "value"},
    {toml::TokenType::TOKEN_KEY, ""},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "blank"},

    {toml::TokenType::TOKEN_KEY, "name"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "Orange"},

    {toml::TokenType::TOKEN_KEY, "physical"},
    toml::tokens::DOT,
    {toml::TokenType::TOKEN_KEY, "color"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "orange"},

    {toml::TokenType::TOKEN_KEY, "physical"},
    toml::tokens::DOT,
    {toml::TokenType::TOKEN_KEY, "shape"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "round"},

    {toml::TokenType::TOKEN_KEY, "site"},
    toml::tokens::DOT,
    {toml::TokenType::TOKEN_KEY, "google.com"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_BOOLEAN, "true"},

    // Integer values
    {toml::TokenType::TOKEN_KEY, "int1"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "+99"},
    {toml::TokenType::TOKEN_KEY, "int2"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "42"},
    {toml::TokenType::TOKEN_KEY, "int3"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "0"},
    {toml::TokenType::TOKEN_KEY, "int4"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "-17"},
    {toml::TokenType::TOKEN_KEY, "int5"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "1_000"},
    {toml::TokenType::TOKEN_KEY, "int6"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "5_349_221"},
    {toml::TokenType::TOKEN_KEY, "int7"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "53_49_221"},
    {toml::TokenType::TOKEN_KEY, "int8"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "1_2_3_4_5"},

    // Hexadecimal
    {toml::TokenType::TOKEN_KEY, "hex1"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "0xDEADBEEF"},
    {toml::TokenType::TOKEN_KEY, "hex2"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "0xdeadbeef"},
    {toml::TokenType::TOKEN_KEY, "hex3"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "0xdead_beef"},

    // Octal
    {toml::TokenType::TOKEN_KEY, "oct1"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "0o01234567"},
    {toml::TokenType::TOKEN_KEY, "oct2"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "0o755"},

    // Binary
    {toml::TokenType::TOKEN_KEY, "bin1"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "0b11010110"},

    // Floats
    {toml::TokenType::TOKEN_KEY, "flt1"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_FLOAT, "+1.0"},
    {toml::TokenType::TOKEN_KEY, "flt2"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_FLOAT, "3.1415"},
    {toml::TokenType::TOKEN_KEY, "flt3"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_FLOAT, "-0.01"},
    {toml::TokenType::TOKEN_KEY, "flt4"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_FLOAT, "5e+22"},
    {toml::TokenType::TOKEN_KEY, "flt5"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_FLOAT, "1e06"},
    {toml::TokenType::TOKEN_KEY, "flt6"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_FLOAT, "-2E-2"},
    {toml::TokenType::TOKEN_KEY, "flt7"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_FLOAT, "6.626e-34"},
    {toml::TokenType::TOKEN_KEY, "flt8"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_FLOAT, "224_617.445_991_228"},

    // Special floats
    {toml::TokenType::TOKEN_KEY, "sf1"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_FLOAT, "inf"},
    {toml::TokenType::TOKEN_KEY, "sf2"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_FLOAT, "+inf"},
    {toml::TokenType::TOKEN_KEY, "sf3"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_FLOAT, "-inf"},
    {toml::TokenType::TOKEN_KEY, "sf4"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_FLOAT, "nan"},
    {toml::TokenType::TOKEN_KEY, "sf5"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_FLOAT, "+nan"},
    {toml::TokenType::TOKEN_KEY, "sf6"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_FLOAT, "-nan"},

    // Booleans
    {toml::TokenType::TOKEN_KEY, "bool1"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_BOOLEAN, "true"},
    {toml::TokenType::TOKEN_KEY, "bool2"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_BOOLEAN, "false"},

    // Strings
    {toml::TokenType::TOKEN_KEY, "str1"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "The quick brown fox jumps over the lazy dog."},
    {toml::TokenType::TOKEN_KEY, "str2"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "The quick brown fox jumps over the lazy dog."},
    {toml::TokenType::TOKEN_KEY, "str3"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "The quick brown fox jumps over the lazy dog."},

    {toml::TokenType::TOKEN_KEY, "str4"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, R"(Here are two quotation marks: "". Simple enough.)"},
    {toml::TokenType::TOKEN_KEY, "str5"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, R"(Here are three quotation marks: """.)"},
    {toml::TokenType::TOKEN_KEY, "str6"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, R"(Here are fifteen quotation marks: """"""""""""""".)"},

    {toml::TokenType::TOKEN_KEY, "winpath"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, R"(C:\Users\nodejs\templates)"},
    {toml::TokenType::TOKEN_KEY, "winpath2"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, R"(\\ServerX\admin$\system32\)"},
    {toml::TokenType::TOKEN_KEY, "quoted"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, R"(Tom "Dubs" Preston-Werner)"},
    {toml::TokenType::TOKEN_KEY, "regex"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, R"(<\i\c*\s*>)"},

    {toml::TokenType::TOKEN_KEY, "regex2"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, R"(I [dw]on't need \d{2} apples)"},
    {toml::TokenType::TOKEN_KEY, "lines"},
    toml::tokens::EQUAL,
    {
      toml::TokenType::TOKEN_STRING,
      "The first newline is\ntrimmed in raw strings.\n   All other whitespace\n   is preserved.\n"
    },
    {toml::TokenType::TOKEN_KEY, "quot15"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, R"(Here are fifteen quotation marks: """"""""""""""")"},
    {toml::TokenType::TOKEN_KEY, "apos15"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "Here are fifteen apostrophes: '''''''''''''''"},

    // Offset Date-Time
    {toml::TokenType::TOKEN_KEY, "odt1"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_OFFSET_DATETIME, "1979-05-27T07:32:00Z"},
    {toml::TokenType::TOKEN_KEY, "odt2"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_OFFSET_DATETIME, "1979-05-27T00:32:00-07:00"},
    {toml::TokenType::TOKEN_KEY, "odt3"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_OFFSET_DATETIME, "1979-05-27T00:32:00.999999-07:00"},
    {toml::TokenType::TOKEN_KEY, "odt4"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_OFFSET_DATETIME, "1979-05-27 07:32:00Z"},

    // Local Date-Time
    {toml::TokenType::TOKEN_KEY, "ldt1"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_LOCAL_DATETIME, "1979-05-27T07:32:00"},
    {toml::TokenType::TOKEN_KEY, "ldt2"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_LOCAL_DATETIME, "1979-05-27T00:32:00.999999"},

    // Local Date
    {toml::TokenType::TOKEN_KEY, "ld1"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_LOCAL_DATE, "1979-05-27"},

    // Local Time
    {toml::TokenType::TOKEN_KEY, "lt1"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_LOCAL_TIME, "07:32:00"},
    {toml::TokenType::TOKEN_KEY, "lt2"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_LOCAL_TIME, "00:32:00.999999"},

    // Arrays
    {toml::TokenType::TOKEN_KEY, "integers"},
    toml::tokens::EQUAL,
    toml::tokens::LBRACKET,
    {toml::TokenType::TOKEN_INTEGER, "1"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_INTEGER, "2"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_INTEGER, "3"},
    toml::tokens::RBRACKET,

    {toml::TokenType::TOKEN_KEY, "colors"},
    toml::tokens::EQUAL,
    toml::tokens::LBRACKET,
    {toml::TokenType::TOKEN_STRING, "red"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_STRING, "yellow"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_STRING, "green"},
    toml::tokens::RBRACKET,

    {toml::TokenType::TOKEN_KEY, "nested_arrays_of_ints"},
    toml::tokens::EQUAL,
    toml::tokens::LBRACKET,
    toml::tokens::LBRACKET,
    {toml::TokenType::TOKEN_INTEGER, "1"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_INTEGER, "2"},
    toml::tokens::RBRACKET,
    toml::tokens::COMMA,
    toml::tokens::LBRACKET,
    {toml::TokenType::TOKEN_INTEGER, "3"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_INTEGER, "4"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_INTEGER, "5"},
    toml::tokens::RBRACKET,
    toml::tokens::RBRACKET,

    {toml::TokenType::TOKEN_KEY, "nested_mixed_array"},
    toml::tokens::EQUAL,
    toml::tokens::LBRACKET,
    toml::tokens::LBRACKET,
    {toml::TokenType::TOKEN_INTEGER, "1"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_INTEGER, "2"},
    toml::tokens::RBRACKET,
    toml::tokens::COMMA,
    toml::tokens::LBRACKET,
    {toml::TokenType::TOKEN_STRING, "a"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_STRING, "b"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_STRING, "c"},
    toml::tokens::RBRACKET,
    toml::tokens::RBRACKET,

    {toml::TokenType::TOKEN_KEY, "string_array"},
    toml::tokens::EQUAL,
    toml::tokens::LBRACKET,
    {toml::TokenType::TOKEN_STRING, "all"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_STRING, "strings"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_STRING, "are the same"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_STRING, "type"},
    toml::tokens::RBRACKET,

    {toml::TokenType::TOKEN_KEY, "numbers"},
    toml::tokens::EQUAL,
    toml::tokens::LBRACKET,
    {toml::TokenType::TOKEN_FLOAT, "0.1"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_FLOAT, "0.2"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_FLOAT, "0.5"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_INTEGER, "1"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_INTEGER, "2"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_INTEGER, "5"},
    toml::tokens::RBRACKET,

    {toml::TokenType::TOKEN_KEY, "integers2"},
    toml::tokens::EQUAL,
    toml::tokens::LBRACKET,
    {toml::TokenType::TOKEN_INTEGER, "1"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_INTEGER, "2"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_INTEGER, "3"},
    toml::tokens::RBRACKET,

    {toml::TokenType::TOKEN_KEY, "integers3"},
    toml::tokens::EQUAL,
    toml::tokens::LBRACKET,
    {toml::TokenType::TOKEN_INTEGER, "1"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_INTEGER, "2"},
    toml::tokens::COMMA,
    toml::tokens::RBRACKET,

    // Tables
    toml::tokens::LBRACKET,
    {toml::TokenType::TOKEN_KEY, "table"},
    toml::tokens::RBRACKET,

    toml::tokens::LBRACKET,
    {toml::TokenType::TOKEN_KEY, "table-1"},
    toml::tokens::RBRACKET,
    {toml::TokenType::TOKEN_KEY, "key1"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "some string"},
    {toml::TokenType::TOKEN_KEY, "key2"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "123"},

    toml::tokens::LBRACKET,
    {toml::TokenType::TOKEN_KEY, "table-2"},
    toml::tokens::RBRACKET,
    {toml::TokenType::TOKEN_KEY, "key1"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "another string"},
    {toml::TokenType::TOKEN_KEY, "key2"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "456"},

    toml::tokens::LBRACKET,
    {toml::TokenType::TOKEN_KEY, "dog"},
    toml::tokens::DOT,
    {toml::TokenType::TOKEN_KEY, "tater.man"},
    toml::tokens::RBRACKET,
    {toml::TokenType::TOKEN_KEY, "type"},
    toml::tokens::DOT,
    {toml::TokenType::TOKEN_KEY, "name"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "pug"},

    // Array of Tables
    toml::tokens::LDBRACKET,
    {toml::TokenType::TOKEN_KEY, "products"},
    toml::tokens::RDBRACKET,
    {toml::TokenType::TOKEN_KEY, "name"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "Hammer"},
    {toml::TokenType::TOKEN_KEY, "sku"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "738594937"},

    toml::tokens::LDBRACKET,
    {toml::TokenType::TOKEN_KEY, "products"},
    toml::tokens::RDBRACKET,

    toml::tokens::LDBRACKET,
    {toml::TokenType::TOKEN_KEY, "products"},
    toml::tokens::RDBRACKET,
    {toml::TokenType::TOKEN_KEY, "name"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "Nail"},
    {toml::TokenType::TOKEN_KEY, "sku"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "284758393"},
    {toml::TokenType::TOKEN_KEY, "color"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "gray"},

    toml::tokens::LDBRACKET,
    {toml::TokenType::TOKEN_KEY, "fruits"},
    toml::tokens::RDBRACKET,
    {toml::TokenType::TOKEN_KEY, "name"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "apple"},

    toml::tokens::LBRACKET,
    {toml::TokenType::TOKEN_KEY, "fruits"},
    toml::tokens::DOT,
    {toml::TokenType::TOKEN_KEY, "physical"},
    toml::tokens::RBRACKET,
    {toml::TokenType::TOKEN_KEY, "color"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "red"},
    {toml::TokenType::TOKEN_KEY, "shape"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "round"},

    toml::tokens::LDBRACKET,
    {toml::TokenType::TOKEN_KEY, "fruits"},
    toml::tokens::DOT,
    {toml::TokenType::TOKEN_KEY, "varieties"},
    toml::tokens::RDBRACKET,
    {toml::TokenType::TOKEN_KEY, "name"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "red delicious"},

    toml::tokens::LDBRACKET,
    {toml::TokenType::TOKEN_KEY, "fruits"},
    toml::tokens::DOT,
    {toml::TokenType::TOKEN_KEY, "varieties"},
    toml::tokens::RDBRACKET,
    {toml::TokenType::TOKEN_KEY, "name"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "granny smith"},

    toml::tokens::LDBRACKET,
    {toml::TokenType::TOKEN_KEY, "fruits"},
    toml::tokens::RDBRACKET,
    {toml::TokenType::TOKEN_KEY, "name"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "banana"},

    toml::tokens::LDBRACKET,
    {toml::TokenType::TOKEN_KEY, "fruits"},
    toml::tokens::DOT,
    {toml::TokenType::TOKEN_KEY, "varieties"},
    toml::tokens::RDBRACKET,
    {toml::TokenType::TOKEN_KEY, "name"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "plantain"},

    // Inline Tables
    {toml::TokenType::TOKEN_KEY, "name"},
    toml::tokens::EQUAL,
    toml::tokens::LBRACE,
    {toml::TokenType::TOKEN_KEY, "first"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "Tom"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_KEY, "last"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "Preston-Werner"},
    toml::tokens::RBRACE,

    {toml::TokenType::TOKEN_KEY, "point"},
    toml::tokens::EQUAL,
    toml::tokens::LBRACE,
    {toml::TokenType::TOKEN_KEY, "x"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "1"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_KEY, "y"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "2"},
    toml::tokens::RBRACE,

    {toml::TokenType::TOKEN_KEY, "animal"},
    toml::tokens::EQUAL,
    toml::tokens::LBRACE,
    {toml::TokenType::TOKEN_KEY, "type"},
    toml::tokens::DOT,
    {toml::TokenType::TOKEN_KEY, "name"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_STRING, "pug"},
    toml::tokens::RBRACE,

    {toml::TokenType::TOKEN_KEY, "points"},
    toml::tokens::EQUAL,
    toml::tokens::LBRACKET,
    toml::tokens::LBRACE,
    {toml::TokenType::TOKEN_KEY, "x"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "1"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_KEY, "y"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "2"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_KEY, "z"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "3"},
    toml::tokens::RBRACE,
    toml::tokens::COMMA,
    toml::tokens::LBRACE,
    {toml::TokenType::TOKEN_KEY, "x"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "7"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_KEY, "y"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "8"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_KEY, "z"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "9"},
    toml::tokens::RBRACE,
    toml::tokens::COMMA,
    toml::tokens::LBRACE,
    {toml::TokenType::TOKEN_KEY, "x"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "2"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_KEY, "y"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "4"},
    toml::tokens::COMMA,
    {toml::TokenType::TOKEN_KEY, "z"},
    toml::tokens::EQUAL,
    {toml::TokenType::TOKEN_INTEGER, "8"},
    toml::tokens::RBRACE,
    toml::tokens::RBRACKET,

    toml::tokens::END_OF_FILE,
  };

  toml::lexer lexer(dedented_input);

  for (const auto &expected_tok: expected_tokens) {
    toml::token actual_tok = lexer.next_token();
    std::cout << "Expected: " << expected_tok << ", Actual: " << actual_tok << std::endl;
    ASSERT_EQ(expected_tok, actual_tok);
  }
}
