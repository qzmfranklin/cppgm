#include "PPToken.h"
#include <gtest/gtest.h>

TEST(PPToken, HeaderName)
{
  const auto tok = PPToken::createHeaderName("stdio.h");
  std::string expected = "header_name stdio.h";
  std::string actually = tok->dumpUTF8String();
  ASSERT_EQ(expected, actually);
}

TEST(PPToken, Identifier)
{
  const auto tok = PPToken::createIdentifier("snake_case_var");
  std::string expected = "identifier snake_case_var";
  std::string actually = tok->dumpUTF8String();
  ASSERT_EQ(expected, actually);
}

TEST(PPToken, PPNumber)
{
  const auto tok = PPToken::createPPNumber("-6.23E-32");
  std::string expected = "pp_number -6.23E-32";
  std::string actually = tok->dumpUTF8String();
  ASSERT_EQ(expected, actually);
}

TEST(PPToken, CharacterLiteral)
{
  const auto tok = PPToken::createCharacterLiteral(U"U\'ひ\'");
  std::string expected = "character_literal U'ひ'";
  std::string actually = tok->dumpUTF8String();
  ASSERT_EQ(expected, actually);
}

TEST(PPToken, UserDefinedCharacterLiteral)
{
  const auto tok = PPToken::createUserDefinedCharacterLiteral(U"TBD");
  std::string expected = "user_defined_character_literal TBD";
  std::string actually = tok->dumpUTF8String();
  ASSERT_EQ(expected, actually);
}

TEST(PPToken, StringLiteral)
{
  const auto tok = PPToken::createStringLiteral(U"Hiragana(平仮名,ひらがな)");
  std::string expected = "string_literal Hiragana(平仮名,ひらがな)";
  std::string actually = tok->dumpUTF8String();
  ASSERT_EQ(expected, actually);
}

TEST(PPToken, UserDefinedStringLiteral)
{
  const auto tok = PPToken::createUserDefinedStringLiteral(U"Hiragana(平仮名,ひらがな)");
  std::string expected = "user_defined_string_literal Hiragana(平仮名,ひらがな)";
  std::string actually = tok->dumpUTF8String();
  ASSERT_EQ(expected, actually);
}

TEST(PPToken, PreprocessingOpOrPunc)
{
  const auto tok = PPToken::createPreprocessingOpOrPunc(">>");
  std::string expected = "preprocessing_op_or_punc >>";
  std::string actually = tok->dumpUTF8String();
  ASSERT_EQ(expected, actually);
}

TEST(PPToken, NonWhitespaceChar)
{
  const auto tok = PPToken::createNonWhitespaceChar(U"😈");
  std::string expected = "non_whitespace_char 😈";
  std::string actually = tok->dumpUTF8String();
  ASSERT_EQ(expected, actually);
}

TEST(PPToken, NewLine)
{
  const auto tok = PPToken::createNewLine();
  std::string expected = "new_line ";
  std::string actually = tok->dumpUTF8String();
  ASSERT_EQ(expected, actually);
}

TEST(PPToken, WhitespaceSequence)
{
  const auto tok = PPToken::createWhitespaceSequence(UR"(djfi  jdie )");
  std::string expected = R"(whitespace_sequence djfi  jdie )";
  std::string actually = tok->dumpUTF8String();
  ASSERT_EQ(expected, actually);
}
