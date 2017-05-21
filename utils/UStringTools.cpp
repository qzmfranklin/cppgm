#include "UStringTools.h"
#include <assert.h>
#include <unicode/unistr.h>

char32_t *UStringTools::_ch32buf = nullptr;
uint32_t UStringTools::_ch32bufCapacity = 0;

std::string UStringTools::u32_to_u8(const std::u32string& u32str)
{
  const icu::UnicodeString ustring = icu::UnicodeString::fromUTF32(
        reinterpret_cast<const UChar32*>(u32str.data()), u32str.length());
  std::string u8str;
  ustring.toUTF8String<std::string>(u8str);
  return u8str;
}

std::u32string UStringTools::u8_to_u32(const std::string& u8str)
{
  const icu::UnicodeString ustring = icu::UnicodeString::fromUTF8(u8str);
  const uint32_t char32length = ustring.countChar32();;

  // Prepare buffer
  if (_ch32buf == nullptr) {
    _ch32buf = new char32_t[char32length];
    _ch32bufCapacity = char32length;
  } else if (_ch32bufCapacity < char32length) {
    delete []_ch32buf;
    _ch32buf = new char32_t[char32length];
    _ch32bufCapacity = char32length;
  }

  UErrorCode err;
  ustring.toUTF32(reinterpret_cast<UChar32*>(_ch32buf), char32length, err);
  assert(U_SUCCESS(err));

  return std::u32string(_ch32buf, char32length);
}

void UStringTools::freeBuffer()
{
  if (_ch32buf != nullptr) {
    delete []_ch32buf;
    _ch32buf = nullptr;
    _ch32bufCapacity = 0;
  }
}
