#ifndef PPCodePointCheck_h
#define PPCodePointCheck_h

// Yes or no checks of single code point
class PPCodePointCheck {
public:
  static bool isBasicSourceCharacter(const char32_t);
  static bool isDigit(const char32_t);
  static bool isNondigit(const char32_t);
  static bool isInAnnexE1(const char32_t);
  static bool isInAnnexE2(const char32_t);
  static bool isBinaryDigit(const char32_t);
  static bool isOctalDigit(const char32_t);
  static bool isHexadecimalDigit(const char32_t);
  static bool isSimpleEscapeChar(const char32_t);
  static bool isSign(const char32_t);

  // Why not isHChar()?
  //
  //   It is not possible to implement isSChar() and isRChar() without using
  //   contextual information.
  //
  //   h-char, q-char, s-char, r-char, d-char are basically the basic source
  //   character char set with some exceptions. It would be a lot faster to just
  //   exclude the wrong chars.
  //
  // CAVEAT:
  //
  //   If isNotSChar() or isNotRChar() returns false, the code point is NOT
  //   guaranteed to be an s-char or an r-char, respectively.
  static bool isNotHChar(const char32_t);
  static bool isNotQChar(const char32_t);
  static bool isNotSChar(const char32_t);
  static bool isNotRChar(const char32_t);
  static bool isNotDChar(const char32_t);
};

#endif /* end of include guard */
