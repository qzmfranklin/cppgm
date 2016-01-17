#include "PPUTF32Stream.h"
#include <gtest/gtest.h>
#include <string>

TEST(PPUTF32Stream, autoAppendEndOfFileNewline)
{
  const std::string u8str = R"(pure text)";
  auto stream = std::make_shared<PPUTF32Stream>(u8str);

  for (int i = 0; i < u8str.length(); i++) {
    ASSERT_FALSE(stream->isEmpty());
    ASSERT_EQ(static_cast<const char32_t>(u8str[i]), stream->getChar32());
    stream->toNext();
  }

  ASSERT_FALSE(stream->isEmpty());
  ASSERT_EQ(U'\n', stream->getChar32());
  stream->toNext();

  ASSERT_TRUE(stream->isEmpty());
}

TEST(PPUTF32Stream, isEmpty)
{
  auto stream = std::make_shared<PPUTF32Stream>(R"(N)");
  ASSERT_FALSE(stream->isEmpty());
  ASSERT_EQ(U'N', stream->getChar32());
  stream->toNext();

  ASSERT_FALSE(stream->isEmpty());
  ASSERT_EQ(U'\n', stream->getChar32());
  stream->toNext();

  ASSERT_TRUE(stream->isEmpty());
}

TEST(PPUTF32Stream, getChar32)
{
  const std::string u8str = R"(Not a problem)";
  auto stream = std::make_shared<PPUTF32Stream>(u8str);

  for (int i = 0; i < u8str.length(); i++) {
    ASSERT_FALSE(stream->isEmpty());
    ASSERT_EQ(static_cast<const char32_t>(u8str[i]), stream->getChar32());
    stream->toNext();
  }

  ASSERT_FALSE(stream->isEmpty());
  ASSERT_EQ(U'\n', stream->getChar32());
  stream->toNext();

  ASSERT_TRUE(stream->isEmpty());
}

TEST(PPUTF32Stream, getUTF32String)
{
}

TEST(PPUTF32Stream, getRawText)
{
}
