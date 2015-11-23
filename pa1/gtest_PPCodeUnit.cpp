#include "PPCodeUnit.h"
#include <gtest/gtest.h>

TEST(PPCodeUnit, CodePoint)
{
  const auto unit = PPCodeUnit::createCodePoint('j');
  ASSERT_EQ(PPCodeUnitType::CodePoint,  unit->getType());
  ASSERT_EQ(static_cast<char32_t>('j'), unit->getChar32());
  ASSERT_EQ(std::string(1, 'j'),        unit->getUTF8String());
  ASSERT_EQ(std::u32string(1, U'j'),    unit->getUTF32String());
}

TEST(PPCodeUnit, UniversalCharacterName)
{
  // The code point is the smiley: 😀
  const auto unit = PPCodeUnit::createUniversalCharacterName(0x1F600, R"(\U1F600)");
  ASSERT_EQ(PPCodeUnitType::UniversalCharacterName, unit->getType());
  ASSERT_EQ(U'😀',                          unit->getChar32());
  ASSERT_EQ(std::string(R"(\U1F600)"),     unit->getUTF8String());
  ASSERT_EQ(std::u32string(UR"(\U1F600)"), unit->getUTF32String());
}

TEST(PPCodeUnit, Comment)
{
  const auto unit = PPCodeUnit::createComment(UR"(/*  \t \n // */)");
  ASSERT_EQ(PPCodeUnitType::Comment,    unit->getType());
  ASSERT_EQ(std::string(R"(/*  \t \n // */)"),     unit->getUTF8String());
  ASSERT_EQ(std::u32string(UR"(/*  \t \n // */)"), unit->getUTF32String());
}
