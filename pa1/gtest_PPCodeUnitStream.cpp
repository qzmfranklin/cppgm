#include "PPCodeUnitStream.h"
#include "PPUTF32Stream.h"
#include <gtest/gtest.h>

TEST(PPCodeUnitStream, ASCIIText)
{
  const std::string src = R"(ASCII text here)";

  auto u32stream = std::make_shared<PPUTF32Stream>(src);
  auto stream = std::make_shared<PPCodeUnitStream>(u32stream);
  ASSERT_FALSE(stream->isEmpty());
  for (int i = 0; i < src.length(); i++) {
    ASSERT_FALSE(stream->isEmpty());
    const std::shared_ptr<PPCodeUnit> unit = stream->getCodeUnit();
    stream->toNext();
    ASSERT_EQ(static_cast<const char32_t>(src[i]), unit->getChar32());
  }
}

TEST(PPCodeUnitStream, AutoAppendEndingNewLine)
{
  const std::string src = R"(pure text)";
  auto u32stream = std::make_shared<PPUTF32Stream>(src);
  auto stream = std::make_shared<PPCodeUnitStream>(u32stream);
  for (int i = 0; i < src.length(); i++) {
    ASSERT_FALSE(stream->isEmpty());
    stream->toNext();
  }

  // The last character should be a new-line.
  ASSERT_FALSE(stream->isEmpty());
  ASSERT_EQ(U'\n', stream->getCodeUnit()->getChar32());
  stream->toNext();
  ASSERT_TRUE(stream->isEmpty());
}

TEST(PPCodeUnitStream, SingleQuad)
{
  const std::string src = R"(\u340F)"; // '㐏'
  auto u32stream = std::make_shared<PPUTF32Stream>(src);
  auto stream = std::make_shared<PPCodeUnitStream>(u32stream);

  ASSERT_FALSE(stream->isEmpty());
  const std::shared_ptr<PPCodeUnit> unit = stream->getCodeUnit();
  ASSERT_EQ(U'\u340F',   unit->getChar32());
  ASSERT_EQ(R"(\u340F)", unit->getUTF8String());
  //ASSERT_EQ(std::u32string(UR"(\u340F)"), unit->getUTF32String());
}

TEST(PPCodeUnitStream, DoubleQuad)
{
  const std::string src = R"(\U0001104D)"; // "𑁍"
  auto u32stream = std::make_shared<PPUTF32Stream>(src);
  auto stream = std::make_shared<PPCodeUnitStream>(u32stream);

  ASSERT_FALSE(stream->isEmpty());
  const std::shared_ptr<PPCodeUnit> unit = stream->getCodeUnit();
  ASSERT_EQ(U'\U0001104D',   unit->getChar32());
  ASSERT_EQ(R"(\U0001104D)", unit->getUTF8String());
  //ASSERT_EQ(std::u32string(UR"(\U0001104D)"), unit->getUTF32String());
}


TEST(PPCodeUnitStream, SimpleMixed)
{
  const std::string src = R"(\u340F
\U0001240F
 Normal program goes here...


)";

  auto u32stream = std::make_shared<PPUTF32Stream>(src);
  auto stream = std::make_shared<PPCodeUnitStream>(u32stream);

  { // \u340F
    ASSERT_FALSE(stream->isEmpty());
    const auto unit = stream->getCodeUnit();
    ASSERT_EQ(PPCodeUnitType::UniversalCharacterName, unit->getType());
    ASSERT_EQ(R"(\u340F)", unit->getUTF8String());
    stream->toNext();
  }

  { // \n
    ASSERT_FALSE(stream->isEmpty());
    const auto unit = stream->getCodeUnit();
    ASSERT_EQ(PPCodeUnitType::ASCIIChar, unit->getType());
    ASSERT_EQ("\n", unit->getUTF8String());
    stream->toNext();
  }

  { // \U0001240F
    ASSERT_FALSE(stream->isEmpty());
    const auto unit = stream->getCodeUnit();
    ASSERT_EQ(PPCodeUnitType::UniversalCharacterName, unit->getType());
    ASSERT_EQ(R"(\U0001240F)", unit->getUTF8String());
    stream->toNext();
  }

  { // \n
    ASSERT_FALSE(stream->isEmpty());
    const auto unit = stream->getCodeUnit();
    ASSERT_EQ(PPCodeUnitType::ASCIIChar, unit->getType());
    ASSERT_EQ("\n", unit->getUTF8String());
    stream->toNext();
  }

  { // basic source characters
    const std::string text = " Normal program goes here...";
    for (int i = 0; i < text.length(); i++) {
      ASSERT_FALSE(stream->isEmpty());
      const auto unit = stream->getCodeUnit();
      ASSERT_EQ(PPCodeUnitType::ASCIIChar, unit->getType());
      ASSERT_EQ(text[i], static_cast<char>(unit->getChar32()));
      stream->toNext();
    }
  }

  { // \n x 3
    for (int i = 0; i < 3; i++) {
      ASSERT_FALSE(stream->isEmpty());
      const auto unit = stream->getCodeUnit();
      ASSERT_EQ(PPCodeUnitType::ASCIIChar, unit->getType());
      ASSERT_EQ("\n", unit->getUTF8String());
      stream->toNext();
    }
  }

  ASSERT_TRUE(stream->isEmpty());
}
