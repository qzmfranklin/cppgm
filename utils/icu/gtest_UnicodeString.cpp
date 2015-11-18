#include "unicode/ucnv.h"
#include <gtest/gtest.h>
#include <stdio.h>

using icu::UnicodeString;

static const char     *utf8cstring  = u8"🛁\tμπάνιο\tμπανιέρα";
static const char16_t *utf16cstring =  u"🛁\tμπάνιο\tμπανιέρα";
static const char32_t *utf32cstring =  U"🛁\tμπάνιο\tμπανιέρα";

TEST(UnicodeString, Utf8ToUtf16)
{
  // Uncomment fprintf to see the actual values
  const UnicodeString str = UnicodeString::fromUTF8(utf8cstring);
  const int32_t length = str.length();
  for (int32_t i = 0; i < length; i++) {
    //fprintf(stderr, "U+%04X ", str[i]);
    ASSERT_EQ(utf16cstring[i], str[i]);
  }
  //fprintf(stderr, "\n");
}
