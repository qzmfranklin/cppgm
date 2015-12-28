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

  const std::string src = R"(12'335.423E+12)";

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
