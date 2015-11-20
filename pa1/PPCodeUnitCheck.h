#ifndef PPCodeUnitCheck_h
#define PPCodeUnitCheck_h

#include "PPCodeUnit.h"

// Yes or no checks on PPCodeUnit.
//
// Why is there a PPCodePointCheck? Shouldn't it be implemented inside of
// PPCodeUnitCheck?
//
//    - Conceptually, PPCodePointCheck works with UTF32 code points, which is
//      one layer below PPCodeUnit.
//
//    - In terms of implementation, PPCodePointCheck works with char32_t whereas
//      PPCodePointCheck works with PPCodeUnit.
//
//    - For example, s-char. An s-char can be a universal-character-name, which
//      is checkable in PPCodeUnitCheck but not in PPCodePointCheck.
//      PPCodeUnitCheck augments the check by considering universal-character-
//      name.
//
//    - Keeping PPCodePointCheck as a standalone class permits its easier use in
//      other projects that requires similar text processing due to its
//      relatively low level interface.
class PPCodeUnitCheck {
public:
  using UnitPtr = std::shared_ptr<PPCodeUnit>;

  static bool isBasicSourceCharacter(const UnitPtr);
  static bool isDigit(const UnitPtr);
  static bool isNondigit(const UnitPtr);
  static bool isIdentifierNondigit(const UnitPtr);
  static bool isInAnnexE1(const UnitPtr);
  static bool isInAnnexE2(const UnitPtr);
  static bool isBinaryDigit(const UnitPtr);
  static bool isOctalDigit(const UnitPtr);
  static bool isHexadecimalDigit(const UnitPtr);
  static bool isSimpleEscapeChar(const UnitPtr);
  static bool isSign(const UnitPtr);

  // Relation to PPCodePointCheck::isNotXChar():
  //
  //   Exactly same for h-char, q-char, r-char, and d-char. The only difference
  //   is in s-char. If an s-char is a universal-character-name, e.g.,
  //   R"(\u034F)", PPCodeUnitCheck::isNotSChar() returns false but
  //   PPCodePointCheck::isNotSChar() returns true.
  static bool isNotHChar(const UnitPtr);
  static bool isNotQChar(const UnitPtr);
  static bool isNotSChar(const UnitPtr);
  static bool isNotRChar(const UnitPtr);
  static bool isNotDChar(const UnitPtr);
};

#endif /* end of include guard */
