#ifndef UTF32StreamIfc_h
#define UTF32StreamIfc_h

#include <string>

// Implementation CAVEAT: If the last code point is not the new-line \n, add it.
class UTF32StreamIfc {
public:

  // Check if the stream is exhausted.
  virtual bool isEmpty() const = 0;

  // Current code point (not code unit).
  virtual char32_t getChar32() const = 0;

  // Move the internal itr to the next code point (not code unit).
  virtual void toNext() = 0;

  virtual std::u32string getUTF32String() const = 0;
  virtual std::string getUTF8String() const = 0;
};

#endif /* end of include guard */
