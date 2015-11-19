#include "PPUTF32Stream.h"
#include <assert.h>

/*
 * The constructor converts the input UTF8 string into an internal UTF32 string
 * for future use. Not very efficient as it copies the buffer around.
 */
PPUTF32Stream::PPUTF32Stream(const std::string &utf8string):
  _str(icu::UnicodeString::fromUTF8(utf8string)),
  _itr(_str)
{
  if (!_str.endsWith(icu::UnicodeString(reinterpret_cast<const UChar*>(u"\n"), 1))) {
    _str.append(u'\n');
    _itr.setText(_str);
  }
}

bool PPUTF32Stream::isEmpty() const
{
  return !_itr.hasNext();
}

char32_t PPUTF32Stream::getChar32() const
{
  assert(!empty());
  return _itr.current32();
}

void PPUTF32Stream::toNext()
{
  assert(_itr.hasNext());
  _itr.next32();
}

std::u32string PPUTF32Stream::getUTF32String() const
{
  int32_t char32length = _str.countChar32();

  if (char32length == 0)
    return std::u32string();

  char32_t *buf = new char32_t[char32length];
  assert(buf);

  UErrorCode err;
  _str.toUTF32(reinterpret_cast<UChar32*>(buf), char32length, err);
  assert(U_SUCCESS(err));

  delete []buf;

  return std::u32string(buf, char32length);
}

std::string PPUTF32Stream::getUTF8String() const
{
  std::string out;
  _str.toUTF8String<std::string>(out);
  return out;
}
