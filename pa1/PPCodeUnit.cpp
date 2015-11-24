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

std::shared_ptr<PPCodeUnitComment>
PPCodeUnit::createComment(const std::u32string &u32str)
{
  return std::make_shared<PPCodeUnitComment>(u32str);
}

std::shared_ptr<PPCodeUnitDigraph>
PPCodeUnit::createDigraph(const std::string &u8str)
{
  return std::make_shared<PPCodeUnitDigraph>(u8str);
}

std::string PPCodeUnitCodePoint::getUTF8String() const
{
  return UStringTools::u32_to_u8(std::u32string(1, _ch32));
}

std::string PPCodeUnitUniversalCharacterName::getUTF8String() const
{
  return _u8string;
}

std::string PPCodeUnitComment::getUTF8String() const
{
  return UStringTools::u32_to_u8(_u32string);
}

std::string PPCodeUnitDigraph::getUTF8String() const
{
  return _u8string;
}
