#ifndef PPCodeUnit_h
#define PPCodeUnit_h

#include <string>

enum class PPCodeUnitType {
  CodePoint,
  UniversalCharacterName,
  WhitespaceCharacter
};

////////////////////////////////////////////////////////////////////////////////
// Forward declaration of subclasses of PPCodeUnit
////////////////////////////////////////////////////////////////////////////////
class PPCodeUnitCodePoint;
class PPCodeUnitUniversalCharacterName;
class PPCodeUnitWhitespaceCharacter;

// Union class of Unicode code point, universal-character-name, and
// whitespace-character.
//
// For a Unicode code point or a universal-character-name, getChar32() is equal
// to the unicode code point it represents.
//
// For a whitespace-character, the value is 0.
class PPCodeUnit {
public:
  PPCodeUnit(const PPCodeUnitType type, const char32_t ch32):
    _type(type), _ch32(ch32) {}

  PPCodeUnitType getType() const { return _type; }
  char32_t getChar32() const { return _ch32; }

  // Virtual methods
  virtual std::string getUTF8String() const = 0;
  virtual std::u32string getUTF32String() const = 0;

  // Factory methods
  // PPCodeUnitCodePoint deals with basic source characters, encodable in a char
  static std::shared_ptr<PPCodeUnitCodePoint> createCodePoint(const char);
  static std::shared_ptr<PPCodeUnitUniversalCharacterName>
    createUniversalCharacterName(const char32_t, const std::u32string&);
  static std::shared_ptr<PPCodeUnitWhitespaceCharacter>
    createWhitespaceCharacter(const std::u32string&);

protected:
  const PPCodeUnitType _type;
  const char32_t _ch32;
};

////////////////////////////////////////////////////////////////////////////////
// Subclasses
////////////////////////////////////////////////////////////////////////////////

class PPCodeUnitCodePoint: public PPCodeUnit {
public:
  PPCodeUnitCodePoint(const char ch):
    PPCodeUnit(PPCodeUnitType::CodePoint, static_cast<char32_t>(ch)) {}
  virtual std::string getUTF8String() const override;
  virtual std::u32string getUTF32String() const override;
};

class PPCodeUnitUniversalCharacterName: public PPCodeUnit {
public:
  PPCodeUnitUniversalCharacterName(const char32_t ch32, const std::u32string &u32str):
    PPCodeUnit(PPCodeUnitType::UniversalCharacterName, ch32), _u32string(u32str) {}
  virtual std::string getUTF8String() const override;
  virtual std::u32string getUTF32String() const override;
private:
  const std::u32string _u32string;
};

class PPCodeUnitWhitespaceCharacter: public PPCodeUnit {
public:
  PPCodeUnitWhitespaceCharacter(const std::u32string &u32str):
    PPCodeUnit(PPCodeUnitType::WhitespaceCharacter, static_cast<char32_t>(0)), _u32string(u32str) {}
  virtual std::string getUTF8String() const override;
  virtual std::u32string getUTF32String() const override;
private:
  const std::u32string _u32string;
};
#endif /* end of include guard */
