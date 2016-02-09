#include "PPToken.h"

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
  "non-whitespace-character",
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
PPToken::createNonWhitespaceChar(const std::string &u8str)
{
  return std::make_shared<PPTokenNonWhitespaceChar>(u8str);
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
