#include "PPCodeUnit.h"
#include "utils/UStringTools.h"

std::shared_ptr<PPCodeUnitCodePoint> PPCodeUnit::createCodePoint(const char ch)
{
  return std::make_shared<PPCodeUnitCodePoint>(ch);
}

std::shared_ptr<PPCodeUnitUniversalCharacterName>
PPCodeUnit::createUniversalCharacterName(const char32_t ch32, const std::string &u8str)
{
  return std::make_shared<PPCodeUnitUniversalCharacterName>(ch32, u8str);
}

std::shared_ptr<PPCodeUnitWhitespaceSequence>
PPCodeUnit::createWhitespaceSequence(const std::u32string &u32str)
{
  return std::make_shared<PPCodeUnitWhitespaceSequence>(u32str);
}

std::string PPCodeUnitCodePoint::getUTF8String() const
{
  return UStringTools::u32_to_u8(getUTF32String());
}

std::u32string PPCodeUnitCodePoint::getUTF32String() const
{
  return std::u32string(1, _ch32);
}

std::string PPCodeUnitUniversalCharacterName::getUTF8String() const
{
  return _u8string;
}

std::u32string PPCodeUnitUniversalCharacterName::getUTF32String() const
{
  return UStringTools::u8_to_u32(_u8string);
}

std::string PPCodeUnitWhitespaceSequence::getUTF8String() const
{
  return UStringTools::u32_to_u8(getUTF32String());
}

std::u32string PPCodeUnitWhitespaceSequence::getUTF32String() const
{
  return _u32string;
}
