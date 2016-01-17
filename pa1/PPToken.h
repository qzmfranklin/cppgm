#ifndef PPToken_h
#define PPToken_h

#include <memory>
#include <string>
#include <vector>

// This file defines the interface of PPToken and its subclasses.

////////////////////////////////////////////////////////////////////////////////
// Token type class
////////////////////////////////////////////////////////////////////////////////
enum class PPTokenType: int {
  // 9 token types defined in the C++ spec
  HeaderName = 0,
  Identifier,
  PPNumber,
  CharacterLiteral,
  UserDefinedCharacterLiteral,
  StringLiteral,
  UserDefinedStringLiteral,
  PreprocessingOpOrPunc,
  NonWhitespaceChar,

  // Additional token types useful for preprocessing
  //
  // new_line: self-exaplaining
  //
  // whitespace_sequence:
  //    maximal sequence of whitespace characters not including line feeds,
  //    (which effectively also includes comments)
  NewLine,
  WhitespaceSequence,
};

////////////////////////////////////////////////////////////////////////////////
// Forward declarations of subclasses
////////////////////////////////////////////////////////////////////////////////
class PPTokenHeaderName;
class PPTokenIdentifier;
class PPTokenPPNumber;
class PPTokenCharacterLiteral;
class PPTokenUserDefinedCharacterLiteral;
class PPTokenStringLiteral;
class PPTokenUserDefinedStringLiteral;
class PPTokenPreprocessingOpOrPunc;
class PPTokenNonWhitespaceChar;
class PPTokenNewLine;
class PPTokenWhitespaceSequence;

////////////////////////////////////////////////////////////////////////////////
// Abstract base class
////////////////////////////////////////////////////////////////////////////////
class PPToken {
public:
  PPToken(const PPTokenType type, const std::string &u8str):
    _type(type), _u8string(u8str) {}

  // Interface
  PPTokenType getType() const { return _type; }

  // Get the corresponding raw text in UTF8
  virtual std::string getRawText() const { return _u8string; }

  // Get human friendly UTF8 string for the given token type
  static std::string getTokenTypeUTF8String(const PPTokenType type);

  // Factory methods
  static std::shared_ptr<PPTokenHeaderName> createHeaderName(const std::string&);
  static std::shared_ptr<PPTokenIdentifier> createIdentifier(const std::string&);
  static std::shared_ptr<PPTokenPPNumber> createPPNumber(const std::string&);
  static std::shared_ptr<PPTokenCharacterLiteral> createCharacterLiteral(const std::string&);
  static std::shared_ptr<PPTokenUserDefinedCharacterLiteral> createUserDefinedCharacterLiteral(const std::string&);
  static std::shared_ptr<PPTokenStringLiteral> createStringLiteral(const std::string&);
  static std::shared_ptr<PPTokenUserDefinedStringLiteral> createUserDefinedStringLiteral(const std::string&);
  static std::shared_ptr<PPTokenPreprocessingOpOrPunc> createPreprocessingOpOrPunc(const std::string&);
  static std::shared_ptr<PPTokenNonWhitespaceChar> createNonWhitespaceChar(const std::string&);
  static std::shared_ptr<PPTokenNewLine> createNewLine();
  static std::shared_ptr<PPTokenWhitespaceSequence> createWhitespaceSequence(const std::string&);

protected:
  const PPTokenType _type;
  const std::string _u8string;

private:
  static const std::vector<std::string> _typeStringList;
};


////////////////////////////////////////////////////////////////////////////////
// Subclasses of PPToken
////////////////////////////////////////////////////////////////////////////////

class PPTokenHeaderName: public PPToken {
public:
  PPTokenHeaderName(const std::string& u8str):
    PPToken(PPTokenType::HeaderName, u8str) {}
};

class PPTokenIdentifier: public PPToken {
public:
  PPTokenIdentifier(const std::string& u8str):
    PPToken(PPTokenType::Identifier, u8str) {}
};

class PPTokenPPNumber: public PPToken {
public:
  PPTokenPPNumber(const std::string& u8str):
    PPToken(PPTokenType::PPNumber, u8str) {}
};

class PPTokenCharacterLiteral: public PPToken {
public:
  PPTokenCharacterLiteral(const std::string& u8str):
    PPToken(PPTokenType::CharacterLiteral, u8str) {}
};

class PPTokenUserDefinedCharacterLiteral: public PPToken {
public:
  PPTokenUserDefinedCharacterLiteral(const std::string& u8str):
    PPToken(PPTokenType::UserDefinedCharacterLiteral, u8str) {}
};

class PPTokenStringLiteral: public PPToken {
public:
  PPTokenStringLiteral(const std::string& u8str):
    PPToken(PPTokenType::StringLiteral, u8str) {}
};

class PPTokenUserDefinedStringLiteral: public PPToken {
public:
  PPTokenUserDefinedStringLiteral(const std::string& u8str):
    PPToken(PPTokenType::UserDefinedStringLiteral, u8str) {}
};

class PPTokenPreprocessingOpOrPunc: public PPToken {
public:
  PPTokenPreprocessingOpOrPunc(const std::string& u8str):
    PPToken(PPTokenType::PreprocessingOpOrPunc, u8str) {}
};

class PPTokenNonWhitespaceChar: public PPToken {
public:
  PPTokenNonWhitespaceChar(const std::string& u8str):
    PPToken(PPTokenType::NonWhitespaceChar, u8str) {}
};

class PPTokenNewLine: public PPToken {
public:
  PPTokenNewLine():
    PPToken(PPTokenType::NewLine, "\n") {}
};

class PPTokenWhitespaceSequence: public PPToken {
public:
  PPTokenWhitespaceSequence(const std::string& u8str):
    PPToken(PPTokenType::WhitespaceSequence, u8str) {}
};

#endif /* end of include guard */
