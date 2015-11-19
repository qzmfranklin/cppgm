#include "PPToken.h"
#include "unicode/ucnv.h"

const std::vector<std::string> PPToken::_typeStringList = {
  "header_name",
  "identifier",
  "pp_number",
  "character_literal",
  "user_defined_character_literal",
  "string_literal",
  "user_defined_string_literal",
  "preprocessing_op_or_punc",
  "non_whitespace_char",
  "new_line",
  "whitespace_sequence",
};

std::string PPToken::getTokenTypeUTF8String(const PPTokenType type)
{
  return _typeStringList[static_cast<int>(type)];
}

std::shared_ptr<PPTokenHeaderName>
PPToken::createHeaderName(const std::string& u8str)
{
  return std::make_shared<PPTokenHeaderName>(u8str);
}

std::shared_ptr<PPTokenIdentifier>
PPToken::createIdentifier(const std::string &u8str)
{
  return std::make_shared<PPTokenIdentifier>(u8str);
}

std::shared_ptr<PPTokenPPNumber>
PPToken::createPPNumber(const std::string &u8str)
{
  return std::make_shared<PPTokenPPNumber>(u8str);
}

std::shared_ptr<PPTokenCharacterLiteral>
PPToken::createCharacterLiteral(const std::u32string &u32str)
{
  return std::make_shared<PPTokenCharacterLiteral>(u32str);
}

std::shared_ptr<PPTokenUserDefinedCharacterLiteral>
PPToken::createUserDefinedCharacterLiteral(const std::u32string &u32str)
{
  return std::make_shared<PPTokenUserDefinedCharacterLiteral>(u32str);
}

std::shared_ptr<PPTokenStringLiteral>
PPToken::createStringLiteral(const std::u32string &u32str)
{
  return std::make_shared<PPTokenStringLiteral>(u32str);
}

std::shared_ptr<PPTokenUserDefinedStringLiteral>
PPToken::createUserDefinedStringLiteral(const std::u32string &u32str)
{
  return std::make_shared<PPTokenUserDefinedStringLiteral>(u32str);
}

std::shared_ptr<PPTokenPreprocessingOpOrPunc>
PPToken::createPreprocessingOpOrPunc(const std::string &u8str)
{
  return std::make_shared<PPTokenPreprocessingOpOrPunc>(u8str);
}

std::shared_ptr<PPTokenNonWhitespaceChar>
PPToken::createNonWhitespaceChar(const std::u32string &u32str)
{
  return std::make_shared<PPTokenNonWhitespaceChar>(u32str);
}

std::shared_ptr<PPTokenNewLine>
PPToken::createNewLine()
{
  return std::make_shared<PPTokenNewLine>();
}

std::shared_ptr<PPTokenWhitespaceSequence>
PPToken::createWhitespaceSequence(const std::u32string &u32str)
{
  return std::make_shared<PPTokenWhitespaceSequence>(u32str);
}

namespace {
std::string _u32_to_u8(const std::u32string &u32str)
{
  icu::UnicodeString ustring = icu::UnicodeString::fromUTF32(
      reinterpret_cast<const UChar32*>(u32str.data()), u32str.length());
  std::string u8str;
  ustring.toUTF8String<std::string>(u8str);
  return u8str;
}
} // namespace

std::string PPTokenHeaderName::dumpUTF8String() const
{
  return getTokenTypeUTF8String(_type) + " " + _u8string;
}

std::string PPTokenIdentifier::dumpUTF8String() const
{
  return getTokenTypeUTF8String(_type) + " " + _u8string;
}

std::string PPTokenPPNumber::dumpUTF8String() const
{
  return getTokenTypeUTF8String(_type) + " " + _u8string;
}

std::string PPTokenCharacterLiteral::dumpUTF8String() const
{
  return getTokenTypeUTF8String(_type) + " " + _u32_to_u8(_u32string);
}

std::string PPTokenUserDefinedCharacterLiteral::dumpUTF8String() const
{
  return getTokenTypeUTF8String(_type) + " " + _u32_to_u8(_u32string);
}

std::string PPTokenStringLiteral::dumpUTF8String() const
{
  return getTokenTypeUTF8String(_type) + " " + _u32_to_u8(_u32string);
}

std::string PPTokenUserDefinedStringLiteral::dumpUTF8String() const
{
  return getTokenTypeUTF8String(_type) + " " + _u32_to_u8(_u32string);
}

std::string PPTokenPreprocessingOpOrPunc::dumpUTF8String() const
{
  return getTokenTypeUTF8String(_type) + " " + _u8string;
}

std::string PPTokenNonWhitespaceChar::dumpUTF8String() const
{
  return getTokenTypeUTF8String(_type) + " " + _u32_to_u8(_u32string);
}

std::string PPTokenNewLine::dumpUTF8String() const
{
  return getTokenTypeUTF8String(_type) + " ";
}

std::string PPTokenWhitespaceSequence::dumpUTF8String() const
{
  return getTokenTypeUTF8String(_type) + " " + _u32_to_u8(_u32string);
}
