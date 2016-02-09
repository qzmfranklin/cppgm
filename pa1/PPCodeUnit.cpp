#include "PPCodePointCheck.h"
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
  char ch = 0;
  if (u8str.length() == 1) {
    if (PPCodePointCheck::isWhitespaceCharacter(u8str[0]))
      ch = u8str[0];
    else
      return nullptr;
  } else if (u8str.length() == 2) {
    if (u8str == "\\\n")
      ch = 0;
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

std::string PPCodeUnitASCIIChar::getRawText() const
{
  return UStringTools::u32_to_u8(std::u32string(1, _ch32));
}

std::string PPCodeUnitNonASCIIChar::getRawText() const
{
  return _u8string;
}

std::string PPCodeUnitNonASCIIChar::getUTF8String() const
{
  return UStringTools::u32_to_u8(std::u32string(1, _ch32));
}

std::string PPCodeUnitWhitespaceCharacter::getRawText() const
{
  return _u8string;
}

std::string PPCodeUnitUniversalCharacterName::getRawText() const
{
  return _u8string;
}

std::string PPCodeUnitUniversalCharacterName::getUTF8String() const
{
  return UStringTools::u32_to_u8(std::u32string(1, _ch32));
}
