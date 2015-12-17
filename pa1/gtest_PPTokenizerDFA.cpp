#include "PPTokenizerDFA.h"
#include "PPUTF32Stream.h"
#include "PPCodeUnitStream.h"
#include <gtest/gtest.h>

TEST(PPTokenizerDFA, HeaderNameH)
{
  const std::string src = R"(#include <stdio.h>)";

  auto u32stream = std::make_shared<PPUTF32Stream>(src);
  auto stream = std::make_shared<PPCodeUnitStream>(u32stream);
  fprintf(stderr,"hellow\n");
  auto ppdfa = std::make_shared<PPTokenizerDFA>(stream);
  fprintf(stderr,"hellow2\n");

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

  ASSERT_TRUE(ppdfa->isEmpty());
}
