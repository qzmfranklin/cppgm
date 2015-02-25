#ifndef _UTILS_UNICODE_H_
#define _UTILS_UNICODE_H_

#include <string>

/*
 * A class for handling single unicode code point.
 */
class unicode {
public:
	uint64_t get_code_point() const;
	std::string getUtf8() const;
	//std::basic_string<uint16_t> getUtf16() const;
	//std::basic_string<uint32_t> gteUtf32() const;

private:
	uint64_t _code_point;
};

#endif /* end of include guard _UTILS_UNICODE_H_*/
