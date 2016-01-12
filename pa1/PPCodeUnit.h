#ifndef PPCodeUnit_h
#define PPCodeUnit_h

#include <memory>
#include <string>

// ASCIIChar is one of the printable characters in the basic source character
// set.
//
// WhitespaceCharacter is one of the following:
//   0x09     horizontal tab, \t
//   0x0A     line feed, \n
//   0x0B     vertical tab
//   0x0C     form feed
//   0x0D     carriage return, \r
//   A backslash \ followed by a newline \n
// The value associated with a WhitespaceCharacter is the same as its ASCII
// character for single characters, or same as the ASCII space character 0x20
// for the backslash followed by a newline.
//
// WhitespaceCharacter and UniversalCharacterName are used for reverting strings
// in raw strings. Note that WhitespaceCharacter and UniversalCharacterName have
// either a control character or a backslash, which are not permitted as a
// d-char. This ensures that we can safely revert WhitespaceCharacter and
// UniversalCharacterName in raw strings without having to worry about collision
// with the enclosing sequence of raw strings.

enum class PPCodeUnitType {
  ASCIIChar,
  WhitespaceCharacter,
  UniversalCharacterName,
};

////////////////////////////////////////////////////////////////////////////////
// Forward declaration of subclasses of PPCodeUnit
////////////////////////////////////////////////////////////////////////////////
class PPCodeUnitASCIIChar;
class PPCodeUnitWhitespaceCharacter;
class PPCodeUnitUniversalCharacterName;

// Union class of Unicode code point, universal-character-name, and
// whitespace-sequence.
//
// For a Unicode code point or a universal-character-name, getChar32() is equal
// to the unicode code point it represents.
//
// For a whitespace-sequence, the value is 0.
class PPCodeUnit {
public:
  PPCodeUnit(const PPCodeUnitType type, const char32_t ch32):
    _type(type), _ch32(ch32) {}

  PPCodeUnitType getType() const { return _type; }
  char32_t getChar32() const { return _ch32; }

  virtual std::string getUTF8String() const = 0;

  // Factory methods
  static std::shared_ptr<PPCodeUnitASCIIChar> createASCIIChar(const char);
  static std::shared_ptr<PPCodeUnitWhitespaceCharacter>
    createWhitespaceCharacter(const std::string&);
  static std::shared_ptr<PPCodeUnitUniversalCharacterName>
    createUniversalCharacterName(const char32_t, const std::string&);

protected:
  const PPCodeUnitType _type;
  const char32_t _ch32;
};

////////////////////////////////////////////////////////////////////////////////
// Subclasses
//
// DO NOT USE THE CONSTRUCTORS BELOW. INSTEAD, USE THE FACTORY METHODS.
////////////////////////////////////////////////////////////////////////////////

class PPCodeUnitASCIIChar: public PPCodeUnit {
public:
  PPCodeUnitASCIIChar(const char ch):
    PPCodeUnit(PPCodeUnitType::ASCIIChar, static_cast<char32_t>(ch)) {}
  virtual std::string getUTF8String() const override;
};

class PPCodeUnitWhitespaceCharacter: public PPCodeUnit {
public:
  PPCodeUnitWhitespaceCharacter(const char ch, const std::string &u8str):
    PPCodeUnit(PPCodeUnitType::WhitespaceCharacter, ch), _u8string(u8str) {}
  virtual std::string getUTF8String() const override;
private:
  const std::string _u8string;
};

class PPCodeUnitUniversalCharacterName: public PPCodeUnit {
public:
  PPCodeUnitUniversalCharacterName(const char32_t ch32, const std::string &u8str):
    PPCodeUnit(PPCodeUnitType::UniversalCharacterName, ch32), _u8string(u8str) {}
  virtual std::string getUTF8String() const override;
private:
  const std::string _u8string;
};

#endif /* end of include guard */
