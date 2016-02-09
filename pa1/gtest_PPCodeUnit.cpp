#include "PPCodeUnit.h"
#include <gtest/gtest.h>

TEST(PPCodeUnit, CodePoint)
{
  const auto unit = PPCodeUnit::createASCIIChar('j');
  ASSERT_EQ(PPCodeUnitType::ASCIIChar,  unit->getType());
  ASSERT_EQ(static_cast<char32_t>('j'), unit->getChar32());
  ASSERT_EQ(std::string(1, 'j'),        unit->getRawText());
}

TEST(PPCodeUnit, WhitespaceCharacterSingleChar)
{
  const std::vector<std::string> singleList = { "\t", "\u000B", "\u000C", " " };
  for (const auto &str: singleList) {
    const auto unit = PPCodeUnit::createWhitespaceCharacter(str);
    ASSERT_EQ(PPCodeUnitType::WhitespaceCharacter,  unit->getType());
    ASSERT_EQ(static_cast<char32_t>(str[0]),        unit->getChar32());
    ASSERT_EQ(str,                                  unit->getRawText());
  }
}

TEST(PPCodeUnit, WhitespaceCharacterLineSplice)
{
  const std::string str = "\\\n";
  const auto unit = PPCodeUnit::createWhitespaceCharacter(str);
  ASSERT_EQ(PPCodeUnitType::WhitespaceCharacter,  unit->getType());
  ASSERT_EQ(U'\0',                                unit->getChar32());
  ASSERT_EQ("\\\n",                               unit->getRawText());
}

TEST(PPCodeUnit, WhitespaceCharacterInvalidInput)
{
  const std::string str = "Hello";
  const auto unit = PPCodeUnit::createWhitespaceCharacter(str);
  ASSERT_EQ(nullptr,  unit);
}

TEST(PPCodeUnit, UniversalCharacterName)
{
  // The code point is the smiley: 😀
  const auto unit = PPCodeUnit::createUniversalCharacterName(0x1F600, R"(\U1F600)");
  ASSERT_EQ(PPCodeUnitType::UniversalCharacterName, unit->getType());
  ASSERT_EQ(U'😀',                      unit->getChar32());
  ASSERT_EQ(std::string(R"(\U1F600)"), unit->getRawText());
}
