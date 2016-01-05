#include "PPTokenizerDFA.h"
#include "PPUTF32Stream.h"
#include "PPCodeUnitStream.h"
#include <gtest/gtest.h>

TEST(PPTokenizerDFA, HeaderNameH)
{
  const std::string src = R"(#include <stdio.h>)";

  auto u32stream = std::make_shared<PPUTF32Stream>(src);
  auto stream = std::make_shared<PPCodeUnitStream>(u32stream);

  auto ppdfa = std::make_shared<PPTokenizerDFA>(stream);

  { // preprocessing-op-or-punc: #
    ASSERT_FALSE(ppdfa->isEmpty());
    const auto tok = ppdfa->getPPToken();
    ASSERT_EQ(PPTokenType::PreprocessingOpOrPunc, tok->getType());
    //ASSERT_EQ("#", tok->getUTF8String());
    ppdfa->toNext();
  }

  { // identifier: include
    ASSERT_FALSE(ppdfa->isEmpty());
    const auto tok = ppdfa->getPPToken();
    ASSERT_EQ(PPTokenType::Identifier, tok->getType());
    ASSERT_EQ("include", tok->getUTF8String());
    ppdfa->toNext();
  }

  { // header: <stdio.h>
    ASSERT_FALSE(ppdfa->isEmpty());
    const auto tok = ppdfa->getPPToken();
    ASSERT_EQ(PPTokenType::HeaderName, tok->getType());
    ASSERT_EQ("<stdio.h>", tok->getUTF8String());
    ppdfa->toNext();
  }

  { // new-line
    ASSERT_FALSE(ppdfa->isEmpty());
    const auto tok = ppdfa->getPPToken();
    ASSERT_EQ(PPTokenType::NewLine, tok->getType());
    ppdfa->toNext();
  }

  ASSERT_TRUE(ppdfa->isEmpty());
}

TEST(PPTokenizerDFA, HeaderNameQ)
{
  const std::string src = R"(#include "stdio.h")";

  auto u32stream = std::make_shared<PPUTF32Stream>(src);
  auto stream = std::make_shared<PPCodeUnitStream>(u32stream);

  auto ppdfa = std::make_shared<PPTokenizerDFA>(stream);

  { // preprocessing-op-or-punc: #
    ASSERT_FALSE(ppdfa->isEmpty());
    const auto tok = ppdfa->getPPToken();
    ASSERT_EQ(PPTokenType::PreprocessingOpOrPunc, tok->getType());
    //ASSERT_EQ("#", tok->getUTF8String());
    ppdfa->toNext();
  }

  { // identifier: include
    ASSERT_FALSE(ppdfa->isEmpty());
    const auto tok = ppdfa->getPPToken();
    ASSERT_EQ(PPTokenType::Identifier, tok->getType());
    ASSERT_EQ("include", tok->getUTF8String());
    ppdfa->toNext();
  }

  { // header: <stdio.h>
    ASSERT_FALSE(ppdfa->isEmpty());
    const auto tok = ppdfa->getPPToken();
    ASSERT_EQ(PPTokenType::HeaderName, tok->getType());
    ASSERT_EQ(R"("stdio.h")", tok->getUTF8String());
    ppdfa->toNext();
  }

  { // new-line
    ASSERT_FALSE(ppdfa->isEmpty());
    const auto tok = ppdfa->getPPToken();
    ASSERT_EQ(PPTokenType::NewLine, tok->getType());
    ppdfa->toNext();
  }

  ASSERT_TRUE(ppdfa->isEmpty());
}

TEST(PPTokenizerDFA, SingleLineComment)
{
  const std::string src = R"( // single line comment)";

  auto u32stream = std::make_shared<PPUTF32Stream>(src);
  auto stream = std::make_shared<PPCodeUnitStream>(u32stream);

  auto ppdfa = std::make_shared<PPTokenizerDFA>(stream);

  // Note that the first space character, 0x20, is completely ignored.

  { // whitespace-sequence: the comment
    ASSERT_FALSE(ppdfa->isEmpty());
    const auto tok = ppdfa->getPPToken();
    ASSERT_EQ(PPTokenType::WhitespaceSequence, tok->getType());
    ASSERT_EQ("// single line comment", tok->getUTF8String());
    ppdfa->toNext();
  }

  { // new-line
    ASSERT_FALSE(ppdfa->isEmpty());
    const auto tok = ppdfa->getPPToken();
    ASSERT_EQ(PPTokenType::NewLine, tok->getType());
    ppdfa->toNext();
  }

  ASSERT_TRUE(ppdfa->isEmpty());
}

TEST(PPTokenizerDFA, MultipleLineComment)
{
  const std::string src = R"(/*
 * multiple line comment // ****
 */)";

  auto u32stream = std::make_shared<PPUTF32Stream>(src);
  auto stream = std::make_shared<PPCodeUnitStream>(u32stream);

  auto ppdfa = std::make_shared<PPTokenizerDFA>(stream);

  // Note that stars and double slashes inside a multiple line comment are
  // treated with no special meaning.

  { // whitespace-sequence: the comment
    const std::string expected_string = src;
    ASSERT_FALSE(ppdfa->isEmpty());
    const auto tok = ppdfa->getPPToken();
    ASSERT_EQ(PPTokenType::WhitespaceSequence, tok->getType());
    ASSERT_EQ(expected_string, tok->getUTF8String());
    ppdfa->toNext();
  }

  { // new-line
    ASSERT_FALSE(ppdfa->isEmpty());
    const auto tok = ppdfa->getPPToken();
    ASSERT_EQ(PPTokenType::NewLine, tok->getType());
    ppdfa->toNext();
  }

  ASSERT_TRUE(ppdfa->isEmpty());
}

TEST(PPTokenizerDFA, PPNumber)
{
  const std::string src = R"(12'335.423E+12_kg)";

  auto u32stream = std::make_shared<PPUTF32Stream>(src);
  auto stream = std::make_shared<PPCodeUnitStream>(u32stream);
  auto ppdfa = std::make_shared<PPTokenizerDFA>(stream);

  { // pp-number
    const std::string expected_string = src;
    ASSERT_FALSE(ppdfa->isEmpty());
    const auto tok = ppdfa->getPPToken();
    ASSERT_EQ(PPTokenType::PPNumber, tok->getType());
    ASSERT_EQ(expected_string, tok->getUTF8String());
    ppdfa->toNext();
  }

  { // new-line
    ASSERT_FALSE(ppdfa->isEmpty());
    const auto tok = ppdfa->getPPToken();
    ASSERT_EQ(PPTokenType::NewLine, tok->getType());
    ppdfa->toNext();
  }

  ASSERT_TRUE(ppdfa->isEmpty());
}

TEST(PPTokenizerDFA, StringLiteral)
{
  const std::vector<std::string> encoding_prefix_list = { "", "u8", "u", "U", "L" };
  const std::vector<std::string> text_list = {
    R"del("hello world\n")del",
    R"del("Fh\x0FAa9xx")del",
    R"del("Fh\2322FAa9xx")del",
    R"del("Fh\0778")del",
    R"del("Fh\078")del",
    R"del(R"(hello world\n)")del",
    R"del(R"(Fh\x0FAa9xx)")del",
    R"del(R"(Fh\2322FAa9xx)")del",
    R"del(R"__(Fh\0778)__")del",
    R"del(R"delimiter(hello world)delimiter")del"
  };

  for (const std::string &encoding_prefix: encoding_prefix_list)
    for (const std::string &text: text_list) {
      const std::string src = encoding_prefix + text;

      auto u32stream = std::make_shared<PPUTF32Stream>(src);
      auto stream = std::make_shared<PPCodeUnitStream>(u32stream);
      auto ppdfa = std::make_shared<PPTokenizerDFA>(stream);

      { // string-literal
        const std::string expected_string = src;
        ASSERT_FALSE(ppdfa->isEmpty());
        const auto tok = ppdfa->getPPToken();
        ASSERT_EQ(PPTokenType::StringLiteral, tok->getType());
        ASSERT_EQ(expected_string, tok->getUTF8String());
        ppdfa->toNext();
      }

      { // new-line
        ASSERT_FALSE(ppdfa->isEmpty());
        const auto tok = ppdfa->getPPToken();
        ASSERT_EQ(PPTokenType::NewLine, tok->getType());
        ppdfa->toNext();
      }

      ASSERT_TRUE(ppdfa->isEmpty());
    }
}

TEST(PPTokenizerDFA, UserDefinedStringLiteral)
{
  const std::vector<std::string> encoding_prefix_list = { "", "u8", "u", "U", "L" };
  const std::vector<std::string> text_list = {
    R"del("hello world\n")del",
    R"del("Fh\x0FAa9xx")del",
    R"del("Fh\2322FAa9xx")del",
    R"del("Fh\0778")del",
    R"del("Fh\078")del",
    R"del(R"(hello world\n)")del",
    R"del(R"(Fh\x0FAa9xx)")del",
    R"del(R"(Fh\2322FAa9xx)")del",
    R"del(R"__(Fh\0778)__")del",
    R"del(R"delimiter(hello world)delimiter")del"
  };
  const std::vector<std::string> ud_suffix_list = { "msg", "kg", "__ms", "s2"};

  for (const std::string &encoding_prefix: encoding_prefix_list)
    for (const std::string &text: text_list)
      for (const std::string &ud_suffix: ud_suffix_list) {
        const std::string src = encoding_prefix + text + ud_suffix;

        auto u32stream = std::make_shared<PPUTF32Stream>(src);
        auto stream = std::make_shared<PPCodeUnitStream>(u32stream);
        auto ppdfa = std::make_shared<PPTokenizerDFA>(stream);

        { // string-literal
          const std::string expected_string = src;
          ASSERT_FALSE(ppdfa->isEmpty());
          const auto tok = ppdfa->getPPToken();
          ASSERT_EQ(PPTokenType::UserDefinedStringLiteral, tok->getType());
          ASSERT_EQ(expected_string, tok->getUTF8String());
          ppdfa->toNext();
        }

        { // new-line
          ASSERT_FALSE(ppdfa->isEmpty());
          const auto tok = ppdfa->getPPToken();
          ASSERT_EQ(PPTokenType::NewLine, tok->getType());
          ppdfa->toNext();
        }

        ASSERT_TRUE(ppdfa->isEmpty());
      }
}

TEST(PPTokenizerDFA, CharacterLiteral)
{
  const std::vector<std::string> encoding_prefix_list = { "", "u8", "u", "U", "L" };
  const std::vector<std::string> text_list = {
    R"del('c')del",
    R"del('\r')del",
    R"del('\127')del",
    R"del('\xFF90')del"
  };

  for (const std::string &encoding_prefix: encoding_prefix_list)
    for (const std::string &text: text_list) {
      const std::string src = encoding_prefix + text;

      auto u32stream = std::make_shared<PPUTF32Stream>(src);
      auto stream = std::make_shared<PPCodeUnitStream>(u32stream);
      auto ppdfa = std::make_shared<PPTokenizerDFA>(stream);

      { // string-literal
        const std::string expected_string = src;
        ASSERT_FALSE(ppdfa->isEmpty());
        const auto tok = ppdfa->getPPToken();
        ASSERT_EQ(PPTokenType::CharacterLiteral, tok->getType());
        ASSERT_EQ(expected_string, tok->getUTF8String());
        ppdfa->toNext();
      }

      { // new-line
        ASSERT_FALSE(ppdfa->isEmpty());
        const auto tok = ppdfa->getPPToken();
        ASSERT_EQ(PPTokenType::NewLine, tok->getType());
        ppdfa->toNext();
      }

      ASSERT_TRUE(ppdfa->isEmpty());
    }
}


TEST(PPTokenizerDFA, UserDefinedCharacterLiteral)
{
  const std::vector<std::string> encoding_prefix_list = { "", "u8", "u", "U", "L" };
  const std::vector<std::string> text_list = {
    R"del('c')del",
    R"del('\r')del",
    R"del('\127')del",
    R"del('\xFF90')del"
  };
  const std::vector<std::string> ud_suffix_list = { "_kg", "ms", "james007" };

  for (const std::string &encoding_prefix: encoding_prefix_list)
    for (const std::string &text: text_list)
      for (const std::string &ud_suffix: ud_suffix_list) {
        const std::string src = encoding_prefix + text + ud_suffix;

        auto u32stream = std::make_shared<PPUTF32Stream>(src);
        auto stream = std::make_shared<PPCodeUnitStream>(u32stream);
        auto ppdfa = std::make_shared<PPTokenizerDFA>(stream);

        { // user-defined-character-literal
          const std::string expected_string = src;
          ASSERT_FALSE(ppdfa->isEmpty());
          const auto tok = ppdfa->getPPToken();
          ASSERT_EQ(PPTokenType::UserDefinedCharacterLiteral, tok->getType());
          ASSERT_EQ(expected_string, tok->getUTF8String());
          ppdfa->toNext();
        }

        { // new-line
          ASSERT_FALSE(ppdfa->isEmpty());
          const auto tok = ppdfa->getPPToken();
          ASSERT_EQ(PPTokenType::NewLine, tok->getType());
          ppdfa->toNext();
        }

        ASSERT_TRUE(ppdfa->isEmpty());
      }
}
