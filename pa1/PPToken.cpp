#include "PPToken.h"
#include "unicode/ucnv.h"

// The order of this list must be exactly the same as the enum PPTokeyType,
// defined in PPToken.h.
const std::vector<std::string> PPToken::_typeStringList = {
  "header-name",
  "identifier",
  "pp-number",
  "character-literal",
  "user-defined-character-literal",
  "string-literal",
  "user-defined-string-literal",
  "preprocessing-op-or-punc",
  "non-whitespace-char",
  "new-line",
  "whitespace-sequence",
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
PPToken::createCharacterLiteral(const std::string &u8str)
{
  return std::make_shared<PPTokenCharacterLiteral>(u8str);
}

std::shared_ptr<PPTokenUserDefinedCharacterLiteral>
PPToken::createUserDefinedCharacterLiteral(const std::string &u8str)
{
  return std::make_shared<PPTokenUserDefinedCharacterLiteral>(u8str);
}

std::shared_ptr<PPTokenStringLiteral>
PPToken::createStringLiteral(const std::string &u8str)
{
  return std::make_shared<PPTokenStringLiteral>(u8str);
}

std::shared_ptr<PPTokenUserDefinedStringLiteral>
PPToken::createUserDefinedStringLiteral(const std::string &u8str)
{
  return std::make_shared<PPTokenUserDefinedStringLiteral>(u8str);
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
PPToken::createWhitespaceSequence(const std::string &u8str)
{
  return std::make_shared<PPTokenWhitespaceSequence>(u8str);
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

std::string PPTokenHeaderName::getUTF8String() const
{
  return _u8string;
}

std::string PPTokenIdentifier::getUTF8String() const
{
  return _u8string;
}

std::string PPTokenPPNumber::getUTF8String() const
{
  return _u8string;
}

std::string PPTokenCharacterLiteral::getUTF8String() const
{
  return _u8string;
}

std::string PPTokenUserDefinedCharacterLiteral::getUTF8String() const
{
  return _u8string;
}

std::string PPTokenStringLiteral::getUTF8String() const
{
  return _u8string;
}

std::string PPTokenUserDefinedStringLiteral::getUTF8String() const
{
  return _u8string;
}

std::string PPTokenPreprocessingOpOrPunc::getUTF8String() const
{
  return _u8string;
}

std::string PPTokenNonWhitespaceChar::getUTF8String() const
{
  return _u32_to_u8(_u32string);
}

std::string PPTokenNewLine::getUTF8String() const
{
  return "\n";
}

std::string PPTokenWhitespaceSequence::getUTF8String() const
{
  return _u8string;
}
