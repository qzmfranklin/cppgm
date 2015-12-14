#ifndef PPUTF32Stream_h
#define PPUTF32Stream_h

#include "UTF32StreamIfc.h"
#include "unicode/ucnv.h"
#include "unicode/schriter.h"
#include <memory>
#include <string>

class PPUTF32Stream: public UTF32StreamIfc {
public:
  PPUTF32Stream(const std::string&);

  virtual bool isEmpty() const override;
  virtual char32_t getChar32() const override;
  virtual void toNext() override;

  virtual std::u32string getUTF32String() const override;
  virtual std::string getUTF8String() const override;

private:
  icu::UnicodeString _str;
  mutable icu::StringCharacterIterator _itr;
};

#endif /* end of include guard */
