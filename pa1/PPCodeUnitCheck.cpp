#include "PPCodePointCheck.h"
#include "PPCodeUnitCheck.h"

bool PPCodeUnitCheck::isBasicSourceCharacter(const UnitPtr unit)
{
  return PPCodePointCheck::isBasicSourceCharacter(unit->getChar32());
}

bool PPCodeUnitCheck::isDigit(const UnitPtr unit)
{
  return PPCodePointCheck::isDigit(unit->getChar32());
}

bool PPCodeUnitCheck::isNondigit(const UnitPtr unit)
{
  return PPCodePointCheck::isNondigit(unit->getChar32());
}

bool PPCodeUnitCheck::isInAnnexE1(const UnitPtr unit)
{
  return PPCodePointCheck::isInAnnexE1(unit->getChar32());
}

bool PPCodeUnitCheck::isInAnnexE2(const UnitPtr unit)
{
  return PPCodePointCheck::isInAnnexE2(unit->getChar32());
}

bool PPCodeUnitCheck::isBinaryDigit(const UnitPtr unit)
{
  return PPCodePointCheck::isBinaryDigit(unit->getChar32());
}

bool PPCodeUnitCheck::isOctalDigit(const UnitPtr unit)
{
  return PPCodePointCheck::isOctalDigit(unit->getChar32());
}

bool PPCodeUnitCheck::isHexadecimalDigit(const UnitPtr unit)
{
  return PPCodePointCheck::isHexadecimalDigit(unit->getChar32());
}

bool PPCodeUnitCheck::isSimpleEscapeChar(const UnitPtr unit)
{
  return PPCodePointCheck::isSimpleEscapeChar(unit->getChar32());
}

bool PPCodeUnitCheck::isSign(const UnitPtr unit)
{
  return PPCodePointCheck::isSign(unit->getChar32());
}

bool PPCodeUnitCheck::isNotHChar(const UnitPtr unit)
{
  return PPCodePointCheck::isNotHChar(unit->getChar32());
}

bool PPCodeUnitCheck::isNotQChar(const UnitPtr unit)
{
  return PPCodePointCheck::isNotQChar(unit->getChar32());
}

bool PPCodeUnitCheck::isNotSChar(const UnitPtr unit)
{
  return PPCodePointCheck::isNotSChar(unit->getChar32())
    && !(unit->getType() == PPCodeUnitType::UniversalCharacterName);
}

bool PPCodeUnitCheck::isNotRChar(const UnitPtr unit)
{
  return PPCodePointCheck::isNotRChar(unit->getChar32());
}

bool PPCodeUnitCheck::isNotDChar(const UnitPtr unit)
{
  return PPCodePointCheck::isNotDChar(unit->getChar32());
}

