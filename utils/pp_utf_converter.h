#ifndef _UTILS_CONVERTUTF_H_
#define _UTILS_CONVERTUTF_H_

/*
 * Wrapper class over the C-functions provided by the Unicode comittee for
 * converting between different encodings.
 */

void foo()
{
	utf_converter g;

	// Decoding utf8 string.
	while(1) {
		const int ch = fetch_char();
		if (ch == -1  ||  ch == EOF)
			break;
		if (g.add_utf8_codeunit(static_cast<uint8_t>(ch)))
			break;
		if (!g.can_emit_codepoint())
			continue;
		const uint32_t codepoint = g.emit_codepoint();
	};

	// Encoding
	while(1) {
	}
}

class pp_utf_converter {

public:
	/*
	 * @return
	 * true  = cannot add code unit any more.
	 * false = successfully added the code unit.
	 */
	bool add_utf8_codeunit(const uint8_t ch);

	/*
	 * @return
	 * true  = the utf8 code unit(s) form a single codepoint.
	 */
	bool can_emit_codepoint() const;

	uint32_t emit_codepoint() const;

	static bool codepoint_to_utf8_string(const uint32_t codepoint, uint8_t *out, int len);

	void clear();

private:

	enum Result {
		SUCCESS,             // conversion successful 
		IN_EXHAUSTED,        // partial character in in, but hit end 
		OUT_EXHAUSTED,       // insufficient space in out for conversion 
		IN_ILLEGAL           // in sequence is illegal/malformed 
	};
	
	enum Mode {
		STRICT = 0,
		LENIENT
	};
	
	Result ConvertUTF8toUTF16 (const uint8_t** inStart, const uint8_t* inEnd, uint16_t** outStart, uint16_t* outEnd, Mode mode);
	Result ConvertUTF8toUTF32 (const uint8_t** inStart, const uint8_t* inEnd, uint32_t** outStart, uint32_t* outEnd, Mode mode);
	Result ConvertUTF16toUTF8 (const uint16_t** inStart, const uint16_t* inEnd, uint8_t** outStart, uint8_t* outEnd, Mode mode);
	Result ConvertUTF32toUTF8 (const uint32_t** inStart, const uint32_t* inEnd, uint8_t** outStart, uint8_t* outEnd, Mode mode);
	Result ConvertUTF16toUTF32 (const uint16_t** inStart, const uint16_t* inEnd, uint32_t** outStart, uint32_t* outEnd, Mode mode);
	Result ConvertUTF32toUTF16 (const uint32_t** inStart, const uint32_t* inEnd, uint16_t** outStart, uint16_t* outEnd, Mode mode);
	bool isLegalUTF8(const uint8_t *in, int length);
	bool isLegalUTF8Sequence(const uint8_t *in, const uint8_t *inEnd);
	bool isLegalUTF8String(const uint8_t **in, const uint8_t *inEnd);
	unsigned getNumBytesForUTF8(uint8_t firstByte);
	
	static const uint32_t UNI_REPLACEMENT_CHAR = 0x0000FFFD;
	static const uint32_t UNI_MAX_BMP          = 0x0000FFFF;
	static const uint32_t UNI_MAX_UTF16        = 0x0010FFFF;
	static const uint32_t UNI_MAX_UTF32        = 0x7FFFFFFF;
	static const uint32_t UNI_MAX_LEGAL_UTF32  = 0x0010FFFF;
	
	static const uint32_t define UNI_MAX_UTF8_BYTES_PER_CODE_POINT = 4;
	
	static const int halfShift  = 10; /* used for shifting by 10 bits */
	
	static const uint32_t halfBase = 0x0010000UL;
	static const uint32_t halfMask = 0x3FFUL;
	
	static const uint32_t UNI_SUR_HIGH_START   = 0xD800;
	static const uint32_t UNI_SUR_HIGH_END     = 0xDBFF;
	static const uint32_t UNI_SUR_LOW_START    = 0xDC00;
	static const uint32_t UNI_SUR_LOW_END      = 0xDFFF;
	
	/*
	 * Index into the table below with the first byte of a UTF-8 sequence to
	 * get the number of trailing bytes that are supposed to follow it.
	 * Note that *legal* UTF-8 values can't have 4 or 5-bytes. The table is
	 * left as-is for anyone who may want to do such conversion, which was
	 * allowed in earlier algorithms.
	 */
	static const char trailingBytesForUTF8[256] = {
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
	};
	
	/*
	 * Magic values subtracted from a buffer value during uint8_t conversion.
	 * This table contains as many values as there might be trailing bytes
	 * in a UTF-8 sequence.
	 */
	static const uint32_t offsetsFromUTF8[6] = {
		0x00000000UL,
		0x00003080UL,
		0x000E2080UL,
		0x03C82080UL,
		0xFA082080UL,
		0x82082080UL
	};
	
	/*
	 * Once the bits are split out into bytes of UTF-8, this is a mask OR-ed
	 * into the first byte, depending on how many bytes follow.  There are
	 * as many entries in this table as there are UTF-8 sequence types.
	 * (I.e., one byte sequence, two byte... etc.). Remember that sequencs
	 * for *legal* UTF-8 will be 4 or fewer bytes total.
	 */
	static const uint8_t firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

}; // class utf_converter;

#endif /* End of include guard */
