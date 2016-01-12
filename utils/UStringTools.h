#ifndef UStringTools_h
#define UStringTools_h

#include <string>

class UStringTools {
public:
  static std::string u32_to_u8(const std::u32string&);
  static std::u32string u8_to_u32(const std::string&);
  static void freeBuffer();
private:
  static char32_t *_ch32buf;
  static uint32_t _ch32bufCapacity;
};

#endif /* end of include guard */
