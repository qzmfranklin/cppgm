#include "PPCodeUnit.h"
#include "utils/UStringTools.h"

std::shared_ptr<PPCodeUnitASCIIChar> PPCodeUnit::createASCIIChar(const char ch)
{
  return std::make_shared<PPCodeUnitASCIIChar>(ch);
}

std::shared_ptr<PPCodeUnitNonASCIIChar>
PPCodeUnit::createNonASCIIChar(const char32_t ch32)
{
  return std::make_shared<PPCodeUnitNonASCIIChar>(ch32);
}

std::shared_ptr<PPCodeUnitWhitespaceCharacter>
PPCodeUnit::createWhitespaceCharacter(const std::string &u8str)
{
  // The whitespace character can only be one of 0x09 - 0x0D, inclusive, or the
  // line-splicing two-char sequence "\\\n". This factory method has to check
  // this. If the input string, u8str, is anything else, return a nullptr to
  // indicate that the method cannot create a valid object with the given input.
  char ch = 0;
  if (u8str.length() == 1) {
    if (u8str[0] != '\t'
        &&  u8str[0] != '\n'
        &&  u8str[0] != static_cast<char>(0x0B)
        &&  u8str[0] != static_cast<char>(0x0C)
        &&  u8str[0] != '\r')
      return nullptr;
    else
      ch = u8str[0];
  } else if (u8str.length() == 2) {
    if (u8str == "\\\n")
      ch = ' ';
    else
      return nullptr;
  } else {
    return nullptr;
  }
  return std::make_shared<PPCodeUnitWhitespaceCharacter>(ch, u8str);
}

std::shared_ptr<PPCodeUnitUniversalCharacterName>
PPCodeUnit::createUniversalCharacterName(const char32_t ch32, const std::string &u8str)
{
  return std::make_shared<PPCodeUnitUniversalCharacterName>(ch32, u8str);
}

std::string PPCodeUnitASCIIChar::getUTF8String() const
{
  return UStringTools::u32_to_u8(std::u32string(1, _ch32));
}

std::string PPCodeUnitNonASCIIChar::getUTF8String() const
{
  return _u8string;
}

std::string PPCodeUnitWhitespaceCharacter::getUTF8String() const
{
  return _u8string;
}

std::string PPCodeUnitUniversalCharacterName::getUTF8String() const
{
  return _u8string;
}
