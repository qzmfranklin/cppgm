#include "PPCodePointCheck.h"

#include <vector>
#include <unordered_set>

namespace {

  const std::unordered_set<char> _basic_source_character_set = {
    ' ',
    '\t',
    '\n',
    static_cast<char>(0x0B), // vertical tab
    static_cast<char>(0x0C), // form feed
    // The carriage return character, '\r' (0x0D), is not allowed

    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',

    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',

    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',

    '_', '{', '}', '[', ']', '#', '(', ')', '<', '>', '%', ':', ';',
    '.', '?', '*', '+', '-', '/', '^', '&', '|', '~', '!', '=', ',',
    '\\',
    '\"',
    '\''
  };

  // h-char: any member of the basic source character set except new-line and >
  // q-char: any member of the basic source character set except new-line and "
  // c-char: any member of the source character set except the single-quote ’,
  //         backslash \, or new-line character
  //         escape-sequence
  //         universal-character-name
  // s-char: any member of the basic source character set except new-line,
  //         double-quote ", and backslash \
  //         escape-sequence
  //         universal-character-name
  // r-char: any member of the basic source character set except
  //         a right parenthesis ) followed by the initial d-char-sequence
  //         (which may be empty) followed by a double quote "
  // d-char: any member of the basic source character set except: space, the
  //         left parenthesis (, the right parenthesis ), the backslash \, and
  //         the control characters representing horizontal tab, vertical tab,
  //         form feed, and newline
  //
  // _X_char_exclude_set (X = h, q, s, r, d, c) is the set of code points that are
  // excluded from the basic source character set.
  //
  // For h-, q-, and d-char, the exclusion is complete.
  // For s- and r-char, the exclusion is complete in the contextual free part.
  const std::vector<char> _h_char_exclude_list = { '\n', '>' };
  const std::vector<char> _q_char_exclude_list = { '\n', '\"' };
  const std::vector<char> _c_char_exclude_list = { '\n', '\'', '\\' };
  const std::vector<char> _s_char_exclude_list = { '\n', '\"', '\\' };
  const std::vector<char> _r_char_exclude_list = { '\n', '\"' };
  const std::vector<char> _d_char_exclude_list = {
    ' ', '(', ')', '\\',
    '\t', '\n', static_cast<char>(0x0B), static_cast<char>(0x0C)
  };

  const std::unordered_set<char> _nondigit_set = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',

    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',

    '_'
  };

  const std::unordered_set<char> _digit_set = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
  };

  const std::unordered_set<char> _simple_escape_char_set = {
    'n', 't', 'v', 'b', 'r', 'f', 'a', '\\', '?', '\'', '\"'
  };

  // See C++ standard 2.11 Identifiers and Appendix/Annex E.1
  const std::vector<std::pair<uint32_t, uint32_t>> AnnexE1_SortedRanges = {
    {0xA8,0xA8},
    {0xAA,0xAA},
    {0xAD,0xAD},
    {0xAF,0xAF},
    {0xB2,0xB5},
    {0xB7,0xBA},
    {0xBC,0xBE},
    {0xC0,0xD6},
    {0xD8,0xF6},
    {0xF8,0xFF},
    {0x100,0x167F},
    {0x1681,0x180D},
    {0x180F,0x1FFF},
    {0x200B,0x200D},
    {0x202A,0x202E},
    {0x203F,0x2040},
    {0x2054,0x2054},
    {0x2060,0x206F},
    {0x2070,0x218F},
    {0x2460,0x24FF},
    {0x2776,0x2793},
    {0x2C00,0x2DFF},
    {0x2E80,0x2FFF},
    {0x3004,0x3007},
    {0x3021,0x302F},
    {0x3031,0x303F},
    {0x3040,0xD7FF},
    {0xF900,0xFD3D},
    {0xFD40,0xFDCF},
    {0xFDF0,0xFE44},
    {0xFE47,0xFFFD},
    {0x10000,0x1FFFD},
    {0x20000,0x2FFFD},
    {0x30000,0x3FFFD},
    {0x40000,0x4FFFD},
    {0x50000,0x5FFFD},
    {0x60000,0x6FFFD},
    {0x70000,0x7FFFD},
    {0x80000,0x8FFFD},
    {0x90000,0x9FFFD},
    {0xA0000,0xAFFFD},
    {0xB0000,0xBFFFD},
    {0xC0000,0xCFFFD},
    {0xD0000,0xDFFFD},
    {0xE0000,0xEFFFD}
  };

  // See C++ standard 2.11 Identifiers and Appendix/Annex E.2
  const std::vector<std::pair<uint32_t, uint32_t>> AnnexE2_SortedRanges = {
    {0x300,0x36F},
    {0x1DC0,0x1DFF},
    {0x20D0,0x20FF},
    {0xFE20,0xFE2F}
  };
}

bool PPCodePointCheck::isBasicSourceCharacter(const char32_t ch32)
{
  return _basic_source_character_set.find(static_cast<char>(ch32)) !=
    _basic_source_character_set.end();
}

bool PPCodePointCheck::isWhitespaceCharacter(const char32_t ch32)
{
  return ch32 == U' '
    ||  ch32 == U'\t'
    ||  ch32 == static_cast<char32_t>(0x0B) // vertical tab
    ||  ch32 == static_cast<char32_t>(0x0C) // form feed;
  ;
}

bool PPCodePointCheck::isDigit(const char32_t ch32)
{
  return _digit_set.find(static_cast<char>(ch32)) !=
    _digit_set.end();
}

bool PPCodePointCheck::isNondigit(const char32_t ch32)
{
  return _nondigit_set.find(static_cast<char>(ch32)) !=
    _nondigit_set.end();
}
bool PPCodePointCheck::isInAnnexE1(const char32_t ch32)
{
  const uint32_t tmp = static_cast<uint32_t>(ch32);
  for (const auto &range: AnnexE1_SortedRanges)
    if (range.first <= tmp  &&  tmp <= range.second)
      return true;
  return false;
}

bool PPCodePointCheck::isInAnnexE2(const char32_t ch32)
{
  const uint32_t tmp = static_cast<uint32_t>(ch32);
  for (const auto &range: AnnexE2_SortedRanges)
    if (range.first <= tmp  &&  tmp <= range.second)
      return true;
  return false;
}

bool PPCodePointCheck::isBinaryDigit(const char32_t ch32)
{
  return ch32 == U'0'  ||  ch32 == U'1';
}

bool PPCodePointCheck::isOctalDigit(const char32_t ch32)
{
  return U'0' <= ch32  &&  ch32 <= U'7';
}

bool PPCodePointCheck::isHexadecimalDigit(const char32_t ch32)
{
  return ( U'0' <= ch32  &&  ch32 <= U'9' )
    || ( U'A' <= ch32  &&  ch32 <= U'F' )
    || ( U'a' <= ch32  &&  ch32 <= U'f' );
}

bool PPCodePointCheck::isSimpleEscapeChar(const char32_t ch32)
{
  return _simple_escape_char_set.find(static_cast<char>(ch32)) !=
    _simple_escape_char_set.end();
}

bool PPCodePointCheck::isSign(const char32_t ch32)
{
  return ch32 == U'+'  ||  ch32 == U'-';
}

bool PPCodePointCheck::isNotHChar(const char32_t ch32)
{
  for (const char ch: _h_char_exclude_list)
    if (static_cast<char>(ch32) == ch)
      return true;
  return !PPCodePointCheck::isBasicSourceCharacter(ch32);
}

bool PPCodePointCheck::isNotQChar(const char32_t ch32)
{
  for (const char ch: _q_char_exclude_list)
    if (static_cast<char>(ch32) == ch)
      return true;
  return !PPCodePointCheck::isBasicSourceCharacter(ch32);
}

bool PPCodePointCheck::isNotCChar(const char32_t ch32)
{
  for (const char ch: _c_char_exclude_list)
    if (static_cast<char>(ch32) == ch)
      return true;
  return !PPCodePointCheck::isBasicSourceCharacter(ch32);
}

bool PPCodePointCheck::isNotSChar(const char32_t ch32)
{
  for (const char ch: _s_char_exclude_list)
    if (static_cast<char>(ch32) == ch)
      return true;
  return !PPCodePointCheck::isBasicSourceCharacter(ch32);
}

bool PPCodePointCheck::isNotRChar(const char32_t ch32)
{
  for (const char ch: _r_char_exclude_list)
    if (static_cast<char>(ch32) == ch)
      return true;
  return !PPCodePointCheck::isBasicSourceCharacter(ch32);
}

bool PPCodePointCheck::isNotDChar(const char32_t ch32)
{
  for (const char ch: _d_char_exclude_list)
    if (static_cast<char>(ch32) == ch)
      return true;
  return !PPCodePointCheck::isBasicSourceCharacter(ch32);
}

