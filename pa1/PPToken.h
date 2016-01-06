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
  PPToken(const PPTokenType type): _type(type) {}

  // Interface
  PPTokenType getType() const { return _type; }

  // Get the corresponding raw text
  virtual std::string getUTF8String() const = 0;

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
  static std::shared_ptr<PPTokenNonWhitespaceChar> createNonWhitespaceChar(const std::u32string&);
  static std::shared_ptr<PPTokenNewLine> createNewLine();
  static std::shared_ptr<PPTokenWhitespaceSequence> createWhitespaceSequence(const std::string&);

protected:
  const PPTokenType _type;

private:
  static const std::vector<std::string> _typeStringList;
};


////////////////////////////////////////////////////////////////////////////////
// Subclasses of PPToken
////////////////////////////////////////////////////////////////////////////////

class PPTokenHeaderName: public PPToken {
public:
  PPTokenHeaderName(const std::string& u8str):
    PPToken(PPTokenType::HeaderName), _u8string(u8str) {}
  virtual std::string getUTF8String() const override;
private:
  const std::string _u8string;
};

class PPTokenIdentifier: public PPToken {
public:
  PPTokenIdentifier(const std::string& u8str):
    PPToken(PPTokenType::Identifier), _u8string(u8str) {}
  virtual std::string getUTF8String() const override;
private:
  const std::string _u8string;
};

class PPTokenPPNumber: public PPToken {
public:
  PPTokenPPNumber(const std::string& u8str):
    PPToken(PPTokenType::PPNumber), _u8string(u8str) {}
  virtual std::string getUTF8String() const override;
private:
  const std::string _u8string;
};

class PPTokenCharacterLiteral: public PPToken {
public:
  PPTokenCharacterLiteral(const std::string& u8str):
    PPToken(PPTokenType::CharacterLiteral), _u8string(u8str) {}
  virtual std::string getUTF8String() const override;
private:
  const std::string _u8string;
};

class PPTokenUserDefinedCharacterLiteral: public PPToken {
public:
  PPTokenUserDefinedCharacterLiteral(const std::string& u8str):
    PPToken(PPTokenType::UserDefinedCharacterLiteral), _u8string(u8str) {}
  virtual std::string getUTF8String() const override;
private:
  const std::string _u8string;
};

class PPTokenStringLiteral: public PPToken {
public:
  PPTokenStringLiteral(const std::string& u8str):
    PPToken(PPTokenType::StringLiteral), _u8string(u8str) {}
  virtual std::string getUTF8String() const override;
private:
  const std::string _u8string;
};

class PPTokenUserDefinedStringLiteral: public PPToken {
public:
  PPTokenUserDefinedStringLiteral(const std::string& u8str):
    PPToken(PPTokenType::UserDefinedStringLiteral), _u8string(u8str) {}
  virtual std::string getUTF8String() const override;
private:
  const std::string _u8string;
};

class PPTokenPreprocessingOpOrPunc: public PPToken {
public:
  PPTokenPreprocessingOpOrPunc(const std::string& u8str):
    PPToken(PPTokenType::PreprocessingOpOrPunc), _u8string(u8str) {}
  virtual std::string getUTF8String() const override;
private:
  const std::string _u8string;
};

class PPTokenNonWhitespaceChar: public PPToken {
public:
  PPTokenNonWhitespaceChar(const std::u32string& u32str):
    PPToken(PPTokenType::NonWhitespaceChar), _u32string(u32str) {}
  virtual std::string getUTF8String() const override;
private:
  const std::u32string _u32string;
};

class PPTokenNewLine: public PPToken {
public:
  PPTokenNewLine():
    PPToken(PPTokenType::NewLine) {}
  virtual std::string getUTF8String() const override;
private:
};

class PPTokenWhitespaceSequence: public PPToken {
public:
  PPTokenWhitespaceSequence(const std::string& u8str):
    PPToken(PPTokenType::WhitespaceSequence), _u8string(u8str) {}
  virtual std::string getUTF8String() const override;
private:
  std::string _u8string;
};

#endif /* end of include guard */
