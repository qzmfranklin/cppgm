#ifndef PPCodeUnit_h
#define PPCodeUnit_h

#include <memory>
#include <string>
#include "utils/UStringTools.h"

// ASCIIChar is one of the printable characters in the basic source character
// set.
//
// WhitespaceCharacter is one of the following:
//   0x09     horizontal tab, \t
//   0x0B     vertical tab
//   0x0C     form feed
//   0x20     whitespace, ' '
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
//
// NonASCIIChar is a unicode code point that cannot be encoded using a single
// UTF8 encoding unit. It is useful for reverting these sequences in raw
// strings. NonASCIIChar is usually encoded using more than one UTF8 units, but
// are treated as a single unit for parsing purpose.
//
// UniversalCharacterName is just the universal-character-name as defined in the
// C++ specification.

enum class PPCodeUnitType {
  ASCIIChar,
  NonASCIIChar,
  WhitespaceCharacter,
  UniversalCharacterName
};

////////////////////////////////////////////////////////////////////////////////
// Forward declaration of subclasses of PPCodeUnit
////////////////////////////////////////////////////////////////////////////////
class PPCodeUnitASCIIChar;
class PPCodeUnitNonASCIIChar;
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

  virtual std::string getRawText() const = 0;
  virtual std::string getUTF8String() const { return getRawText(); }

  // Factory methods
  static std::shared_ptr<PPCodeUnitASCIIChar> createASCIIChar(const char);
  static std::shared_ptr<PPCodeUnitNonASCIIChar>
    createNonASCIIChar(const char32_t);
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
  virtual std::string getRawText() const override;
};

class PPCodeUnitNonASCIIChar: public PPCodeUnit {
public:
  PPCodeUnitNonASCIIChar(const char32_t ch32):
    PPCodeUnit(PPCodeUnitType::NonASCIIChar, ch32),
    _u8string(UStringTools::u32_to_u8(std::u32string(1, ch32))) {}
  virtual std::string getRawText() const override;
  virtual std::string getUTF8String() const override;
private:
  const std::string _u8string;
};

class PPCodeUnitWhitespaceCharacter: public PPCodeUnit {
public:
  PPCodeUnitWhitespaceCharacter(const char ch, const std::string &u8str):
    PPCodeUnit(PPCodeUnitType::WhitespaceCharacter, ch), _u8string(u8str) {}
  virtual std::string getRawText() const override;
private:
  const std::string _u8string;
};

class PPCodeUnitUniversalCharacterName: public PPCodeUnit {
public:
  PPCodeUnitUniversalCharacterName(const char32_t ch32, const std::string &u8str):
    PPCodeUnit(PPCodeUnitType::UniversalCharacterName, ch32), _u8string(u8str) {}
  virtual std::string getRawText() const override;
  virtual std::string getUTF8String() const override;
private:
  const std::string _u8string;
};

#endif /* end of include guard */
