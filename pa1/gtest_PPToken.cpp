#include "PPToken.h"
#include <gtest/gtest.h>

TEST(PPToken, HeaderName)
{
  const std::string src = "stdio.h";
  const auto tok = PPToken::createHeaderName(src);
  ASSERT_EQ(PPTokenType::HeaderName, tok->getType());
  ASSERT_EQ(src, tok->getUTF8String());
}

TEST(PPToken, Identifier)
{
  const std::string src = "snake_case_var";
  const auto tok = PPToken::createIdentifier(src);
  ASSERT_EQ(PPTokenType::Identifier, tok->getType());
  ASSERT_EQ(src, tok->getUTF8String());
}

TEST(PPToken, PPNumber)
{
  const std::string src = "-6.23E-32";
  const auto tok = PPToken::createPPNumber(src);
  ASSERT_EQ(PPTokenType::PPNumber, tok->getType());
  ASSERT_EQ(src, tok->getUTF8String());
}

TEST(PPToken, CharacterLiteral)
{
  const auto tok = PPToken::createCharacterLiteral(R"(U'ひ')");
  ASSERT_EQ(PPTokenType::CharacterLiteral, tok->getType());
  ASSERT_EQ(R"(U'ひ')", tok->getUTF8String());
}

TEST(PPToken, UserDefinedCharacterLiteral)
{
  const auto tok = PPToken::createUserDefinedCharacterLiteral(R"(U'TBD')");
  ASSERT_EQ(PPTokenType::UserDefinedCharacterLiteral, tok->getType());
  ASSERT_EQ(R"(U'TBD')", tok->getUTF8String());
}

TEST(PPToken, StringLiteral)
{
  const auto tok = PPToken::createStringLiteral("Hiragana(平仮名,ひらがな)");
  ASSERT_EQ(PPTokenType::StringLiteral, tok->getType());
  ASSERT_EQ("Hiragana(平仮名,ひらがな)", tok->getUTF8String());
}

TEST(PPToken, UserDefinedStringLiteral)
{
  const auto tok = PPToken::createUserDefinedStringLiteral("Hiragana(平仮名,ひらがな)");
  ASSERT_EQ(PPTokenType::UserDefinedStringLiteral, tok->getType());
  ASSERT_EQ("Hiragana(平仮名,ひらがな)", tok->getUTF8String());
}

TEST(PPToken, PreprocessingOpOrPunc)
{
  const std::string src = ">>";
  const auto tok = PPToken::createPreprocessingOpOrPunc(src);
  ASSERT_EQ(PPTokenType::PreprocessingOpOrPunc, tok->getType());
  ASSERT_EQ(src, tok->getUTF8String());
}

TEST(PPToken, NonWhitespaceChar)
{
  const auto tok = PPToken::createNonWhitespaceChar(U"😈");
  ASSERT_EQ(PPTokenType::NonWhitespaceChar, tok->getType());
  ASSERT_EQ("😈", tok->getUTF8String());
}

TEST(PPToken, NewLine)
{
  const auto tok = PPToken::createNewLine();
  ASSERT_EQ(PPTokenType::NewLine, tok->getType());
  ASSERT_EQ("\n", tok->getUTF8String());
}

TEST(PPToken, WhitespaceSequence)
{
  const auto tok = PPToken::createWhitespaceSequence(R"(djfi  jdie )");
  ASSERT_EQ(PPTokenType::WhitespaceSequence, tok->getType());
  ASSERT_EQ(R"(djfi  jdie )", tok->getUTF8String());
}
