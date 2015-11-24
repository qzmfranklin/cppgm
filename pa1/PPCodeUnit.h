#ifndef PPCodeUnit_h
#define PPCodeUnit_h

#include <string>

enum class PPCodeUnitType {
  ASCIIChar,
  UniversalCharacterName,
  Comment,
  Digraph
};

////////////////////////////////////////////////////////////////////////////////
// Forward declaration of subclasses of PPCodeUnit
////////////////////////////////////////////////////////////////////////////////
class PPCodeUnitASCIIChar;
class PPCodeUnitUniversalCharacterName;
class PPCodeUnitComment;
class PPCodeUnitDigraph;

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
  static std::shared_ptr<PPCodeUnitUniversalCharacterName>
    createUniversalCharacterName(const char32_t, const std::string&);
  static std::shared_ptr<PPCodeUnitComment>
    createComment(const std::u32string&);
  static std::shared_ptr<PPCodeUnitDigraph>
    createDigraph(const std::string&);

protected:
  const PPCodeUnitType _type;
  const char32_t _ch32;
};

////////////////////////////////////////////////////////////////////////////////
// Subclasses
////////////////////////////////////////////////////////////////////////////////

class PPCodeUnitASCIIChar: public PPCodeUnit {
public:
  PPCodeUnitASCIIChar(const char ch):
    PPCodeUnit(PPCodeUnitType::ASCIIChar, static_cast<char32_t>(ch)) {}
  virtual std::string getUTF8String() const override;
};

class PPCodeUnitUniversalCharacterName: public PPCodeUnit {
public:
  PPCodeUnitUniversalCharacterName(const char32_t ch32, const std::string &u8str):
    PPCodeUnit(PPCodeUnitType::UniversalCharacterName, ch32), _u8string(u8str) {}
  virtual std::string getUTF8String() const override;
private:
  const std::string _u8string;
};

class PPCodeUnitComment: public PPCodeUnit {
public:
  PPCodeUnitComment(const std::u32string &u32str):
    PPCodeUnit(PPCodeUnitType::Comment, static_cast<char32_t>(0)), _u32string(u32str) {}
  virtual std::string getUTF8String() const override;
private:
  const std::u32string _u32string;
};

class PPCodeUnitDigraph: public PPCodeUnit {
public:
  PPCodeUnitDigraph(const std::string &u8str):
    PPCodeUnit(PPCodeUnitType::Digraph, static_cast<char32_t>(0)), _u8string(u8str) {}
  virtual std::string getUTF8String() const override;
private:
  const std::string _u8string;
};
#endif /* end of include guard */
