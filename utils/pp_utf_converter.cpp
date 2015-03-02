/*===--- ConvertUTF.c - Universal Character Names conversions ---------------===
 *
 *                     The LLVM Compiler Infrastructure
 *
 * This file is distributed under the University of Illinois Open Source
 * License. See LICENSE.TXT for details.
 *
 *===------------------------------------------------------------------------=*/
/*
 * Copyright 2001-2004 Unicode, Inc.
 * 
 * Disclaimer
 * 
 * This in code is provided as is by Unicode, Inc. No claims are
 * made as to fitness for any particular purpose. No warranties of any
 * kind are expressed or implied. The recipient agrees to determine
 * applicability of information provided. If this file has been
 * purchased on magnetic or optical media from Unicode, Inc., the
 * sole remedy for any claim will be exchange of defective media
 * within 90 days of receipt.
 * 
 * Limitations on Rights to Redistribute This Code
 * 
 * Unicode, Inc. hereby grants the right to freely use the information
 * supplied in this file in the creation of products supporting the
 * Unicode Standard, and to make copies of this file in any form
 * for internal or external distribution as long as this notice
 * remains attached.
 */
/* ---------------------------------------------------------------------

    Conversions between UTF32, UTF-16, and UTF-8. Source code file.
    Author: Mark E. Davis, 1994.
    Rev History: Rick McGowan, fixes & updates May 2001.
    Sept 2001: fixed const & error conditions per
        mods suggested by S. Parent & A. Lillich.
    June 2002: Tim Dodd added detection and handling of incomplete
        in sequences, enhanced error detection, added casts
        to eliminate compiler warnings.
    July 2003: slight mods to back out aggressive FFFE detection.
    Jan 2004: updated switches in from-UTF8 conversions.
    Oct 2004: updated to use UNI_MAX_LEGAL_UTF32 in UTF-32 conversions.

    See the header file "ConvertUTF.h" for complete documentation.

------------------------------------------------------------------------ */

#include "ConvertUTF.h"

/* The interface converts a whole buffer to avoid function-call overhead.
 * Constants have been gathered. Loops & conditionals have been removed as
 * much as possible for efficiency, in favor of drop-through switches.
 * (See "Note A" at the bottom of the file for equivalent code.)
 * If your compiler supports it, the "isLegalUTF8" call can be turned
 * into an inline function.
 */

Result pp_utf_converter::ConvertUTF32toUTF16 (
        const uint32_t** inStart, const uint32_t* inEnd, 
        uint16_t** outStart, uint16_t* outEnd, Mode mode) {
    Result result = conversionOK;
    const uint32_t* in = *inStart;
    uint16_t* out = *outStart;
    while (in < inEnd) {
        uint32_t ch;
        if (out >= outEnd) {
            result = outExhausted; break;
        }
        ch = *in++;
        if (ch <= UNI_MAX_BMP) { /* Target is a character <= 0xFFFF */
            /* UTF-16 surrogate values are illegal in UTF-32; 0xffff or 0xfffe are both reserved values */
            if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
                if (mode == strictConversion) {
                    --in; /* return to the illegal value itself */
                    result = inIllegal;
                    break;
                } else {
                    *out++ = UNI_REPLACEMENT_CHAR;
                }
            } else {
                *out++ = (uint16_t)ch; /* normal case */
            }
        } else if (ch > UNI_MAX_LEGAL_UTF32) {
            if (mode == strictConversion) {
                result = inIllegal;
            } else {
                *out++ = UNI_REPLACEMENT_CHAR;
            }
        } else {
            /* out is a character in range 0xFFFF - 0x10FFFF. */
            if (out + 1 >= outEnd) {
                --in; /* Back up in pointer! */
                result = outExhausted; break;
            }
            ch -= halfBase;
            *out++ = (uint16_t)((ch >> halfShift) + UNI_SUR_HIGH_START);
            *out++ = (uint16_t)((ch & halfMask) + UNI_SUR_LOW_START);
        }
    }
    *inStart = in;
    *outStart = out;
    return result;
}

Result pp_utf_converter::ConvertUTF16toUTF32 (
        const uint16_t** inStart, const uint16_t* inEnd, 
        uint32_t** outStart, uint32_t* outEnd, Mode mode) {
    Result result = conversionOK;
    const uint16_t* in = *inStart;
    uint32_t* out = *outStart;
    uint32_t ch, ch2;
    while (in < inEnd) {
        const uint16_t* oldSource = in; /*  In case we have to back up because of out overflow. */
        ch = *in++;
        /* If we have a surrogate pair, convert to uint32_t first. */
        if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
            /* If the 16 bits following the high surrogate are in the in buffer... */
            if (in < inEnd) {
                ch2 = *in;
                /* If it's a low surrogate, convert to uint32_t. */
                if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
                    ch = ((ch - UNI_SUR_HIGH_START) << halfShift)
                        + (ch2 - UNI_SUR_LOW_START) + halfBase;
                    ++in;
                } else if (mode == strictConversion) { /* it's an unpaired high surrogate */
                    --in; /* return to the illegal value itself */
                    result = inIllegal;
                    break;
                }
            } else { /* We don't have the 16 bits following the high surrogate. */
                --in; /* return to the high surrogate */
                result = inExhausted;
                break;
            }
        } else if (mode == strictConversion) {
            /* UTF-16 surrogate values are illegal in UTF-32 */
            if (ch >= UNI_SUR_LOW_START && ch <= UNI_SUR_LOW_END) {
                --in; /* return to the illegal value itself */
                result = inIllegal;
                break;
            }
        }
        if (out >= outEnd) {
            in = oldSource; /* Back up in pointer! */
            result = outExhausted; break;
        }
        *out++ = ch;
    }
    *inStart = in;
    *outStart = out;
    return result;
}

Result pp_utf_converter::ConvertUTF16toUTF8 (
        const uint16_t** inStart, const uint16_t* inEnd, 
        uint8_t** outStart, uint8_t* outEnd, Mode mode) {
    Result result = conversionOK;
    const uint16_t* in = *inStart;
    uint8_t* out = *outStart;
    while (in < inEnd) {
        uint32_t ch;
        unsigned short bytesToWrite = 0;
        const uint32_t byteMask = 0xBF;
        const uint32_t byteMark = 0x80; 
        const uint16_t* oldSource = in; /* In case we have to back up because of out overflow. */
        ch = *in++;
        /* If we have a surrogate pair, convert to uint32_t first. */
        if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
            /* If the 16 bits following the high surrogate are in the in buffer... */
            if (in < inEnd) {
                uint32_t ch2 = *in;
                /* If it's a low surrogate, convert to uint32_t. */
                if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
                    ch = ((ch - UNI_SUR_HIGH_START) << halfShift)
                        + (ch2 - UNI_SUR_LOW_START) + halfBase;
                    ++in;
                } else if (mode == strictConversion) { /* it's an unpaired high surrogate */
                    --in; /* return to the illegal value itself */
                    result = inIllegal;
                    break;
                }
            } else { /* We don't have the 16 bits following the high surrogate. */
                --in; /* return to the high surrogate */
                result = inExhausted;
                break;
            }
        } else if (mode == strictConversion) {
            /* UTF-16 surrogate values are illegal in UTF-32 */
            if (ch >= UNI_SUR_LOW_START && ch <= UNI_SUR_LOW_END) {
                --in; /* return to the illegal value itself */
                result = inIllegal;
                break;
            }
        }
        /* Figure out how many bytes the result will require */
        if (ch < (uint32_t)0x80) {      bytesToWrite = 1;
        } else if (ch < (uint32_t)0x800) {     bytesToWrite = 2;
        } else if (ch < (uint32_t)0x10000) {   bytesToWrite = 3;
        } else if (ch < (uint32_t)0x110000) {  bytesToWrite = 4;
        } else {                            bytesToWrite = 3;
                                            ch = UNI_REPLACEMENT_CHAR;
        }

        out += bytesToWrite;
        if (out > outEnd) {
            in = oldSource; /* Back up in pointer! */
            out -= bytesToWrite; result = outExhausted; break;
        }
        switch (bytesToWrite) { /* note: everything falls through. */
            case 4: *--out = (uint8_t)((ch | byteMark) & byteMask); ch >>= 6;
            case 3: *--out = (uint8_t)((ch | byteMark) & byteMask); ch >>= 6;
            case 2: *--out = (uint8_t)((ch | byteMark) & byteMask); ch >>= 6;
            case 1: *--out =  (uint8_t)(ch | firstByteMark[bytesToWrite]);
        }
        out += bytesToWrite;
    }
    *inStart = in;
    *outStart = out;
    return result;
}

Result pp_utf_converter::ConvertUTF32toUTF8 (
        const uint32_t** inStart, const uint32_t* inEnd, 
        uint8_t** outStart, uint8_t* outEnd, Mode mode) {
    Result result = conversionOK;
    const uint32_t* in = *inStart;
    uint8_t* out = *outStart;
    while (in < inEnd) {
        uint32_t ch;
        unsigned short bytesToWrite = 0;
        const uint32_t byteMask = 0xBF;
        const uint32_t byteMark = 0x80; 
        ch = *in++;
        if (mode == strictConversion ) {
            /* UTF-16 surrogate values are illegal in UTF-32 */
            if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
                --in; /* return to the illegal value itself */
                result = inIllegal;
                break;
            }
        }
        /*
         * Figure out how many bytes the result will require. Turn any
         * illegally large uint32_t things (> Plane 17) into replacement chars.
         */
        if (ch < (uint32_t)0x80) {      bytesToWrite = 1;
        } else if (ch < (uint32_t)0x800) {     bytesToWrite = 2;
        } else if (ch < (uint32_t)0x10000) {   bytesToWrite = 3;
        } else if (ch <= UNI_MAX_LEGAL_UTF32) {  bytesToWrite = 4;
        } else {                            bytesToWrite = 3;
                                            ch = UNI_REPLACEMENT_CHAR;
                                            result = inIllegal;
        }
        
        out += bytesToWrite;
        if (out > outEnd) {
            --in; /* Back up in pointer! */
            out -= bytesToWrite; result = outExhausted; break;
        }
        switch (bytesToWrite) { /* note: everything falls through. */
            case 4: *--out = (uint8_t)((ch | byteMark) & byteMask); ch >>= 6;
            case 3: *--out = (uint8_t)((ch | byteMark) & byteMask); ch >>= 6;
            case 2: *--out = (uint8_t)((ch | byteMark) & byteMask); ch >>= 6;
            case 1: *--out = (uint8_t) (ch | firstByteMark[bytesToWrite]);
        }
        out += bytesToWrite;
    }
    *inStart = in;
    *outStart = out;
    return result;
}

/*
 * Utility routine to tell whether a sequence of bytes is legal UTF-8.
 * This must be called with the length pre-determined by the first byte.
 * If not calling this from ConvertUTF8to*, then the length can be set by:
 *  length = trailingBytesForUTF8[*in]+1;
 * and the sequence is illegal right away if there aren't that many bytes
 * available.
 * If presented with a length > 4, this returns false.  The Unicode
 * definition of UTF-8 goes up to 4-byte sequences.
 */
bool pp_utf_converter::isLegalUTF8(const uint8_t *in, int length) {
    uint8_t a;
    const uint8_t *srcptr = in+length;
    switch (length) {
    default: return false;
        /* Everything else falls through when "true"... */
    case 4: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
    case 3: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
    case 2: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;

        switch (*in) {
            /* no fall-through in this inner switch */
            case 0xE0: if (a < 0xA0) return false; break;
            case 0xED: if (a > 0x9F) return false; break;
            case 0xF0: if (a < 0x90) return false; break;
            case 0xF4: if (a > 0x8F) return false; break;
            default:   if (a < 0x80) return false;
        }

    case 1: if (*in >= 0x80 && *in < 0xC2) return false;
    }
    if (*in > 0xF4) return false;
    return true;
}

/*
 * Exported function to return whether a UTF-8 sequence is legal or not.
 * This is not used here; it's just exported.
 */
bool pp_utf_converter::isLegalUTF8Sequence(const uint8_t *in, const uint8_t *inEnd) {
    int length = trailingBytesForUTF8[*in]+1;
    if (length > inEnd - in) {
        return false;
    }
    return isLegalUTF8(in, length);
}

/*
 * Exported function to return the total number of bytes in a codepoint
 * represented in UTF-8, given the value of the first byte.
 */
unsigned pp_utf_converter::getNumBytesForUTF8(uint8_t first) {
  return trailingBytesForUTF8[first] + 1;
}

/*
 * Exported function to return whether a UTF-8 string is legal or not.
 * This is not used here; it's just exported.
 */
bool pp_utf_converter::isLegalUTF8String(const uint8_t **in, const uint8_t *inEnd) {
    while (*in != inEnd) {
        int length = trailingBytesForUTF8[**in] + 1;
        if (length > inEnd - *in || !isLegalUTF8(*in, length))
            return false;
        *in += length;
    }
    return true;
}

Result pp_utf_converter::ConvertUTF8toUTF16 (
        const uint8_t** inStart, const uint8_t* inEnd, 
        uint16_t** outStart, uint16_t* outEnd, Mode mode) {
    Result result = conversionOK;
    const uint8_t* in = *inStart;
    uint16_t* out = *outStart;
    while (in < inEnd) {
        uint32_t ch = 0;
        unsigned short extraBytesToRead = trailingBytesForUTF8[*in];
        if (extraBytesToRead >= inEnd - in) {
            result = inExhausted; break;
        }
        /* Do this check whether lenient or strict */
        if (!isLegalUTF8(in, extraBytesToRead+1)) {
            result = inIllegal;
            break;
        }
        /*
         * The cases all fall through. See "Note A" below.
         */
        switch (extraBytesToRead) {
            case 5: ch += *in++; ch <<= 6; /* remember, illegal UTF-8 */
            case 4: ch += *in++; ch <<= 6; /* remember, illegal UTF-8 */
            case 3: ch += *in++; ch <<= 6;
            case 2: ch += *in++; ch <<= 6;
            case 1: ch += *in++; ch <<= 6;
            case 0: ch += *in++;
        }
        ch -= offsetsFromUTF8[extraBytesToRead];

        if (out >= outEnd) {
            in -= (extraBytesToRead+1); /* Back up in pointer! */
            result = outExhausted; break;
        }
        if (ch <= UNI_MAX_BMP) { /* Target is a character <= 0xFFFF */
            /* UTF-16 surrogate values are illegal in UTF-32 */
            if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
                if (mode == strictConversion) {
                    in -= (extraBytesToRead+1); /* return to the illegal value itself */
                    result = inIllegal;
                    break;
                } else {
                    *out++ = UNI_REPLACEMENT_CHAR;
                }
            } else {
                *out++ = (uint16_t)ch; /* normal case */
            }
        } else if (ch > UNI_MAX_UTF16) {
            if (mode == strictConversion) {
                result = inIllegal;
                in -= (extraBytesToRead+1); /* return to the start */
                break; /* Bail out; shouldn't continue */
            } else {
                *out++ = UNI_REPLACEMENT_CHAR;
            }
        } else {
            /* out is a character in range 0xFFFF - 0x10FFFF. */
            if (out + 1 >= outEnd) {
                in -= (extraBytesToRead+1); /* Back up in pointer! */
                result = outExhausted; break;
            }
            ch -= halfBase;
            *out++ = (uint16_t)((ch >> halfShift) + UNI_SUR_HIGH_START);
            *out++ = (uint16_t)((ch & halfMask) + UNI_SUR_LOW_START);
        }
    }
    *inStart = in;
    *outStart = out;
    return result;
}

Result pp_utf_converter::ConvertUTF8toUTF32 (
        const uint8_t** inStart, const uint8_t* inEnd, 
        uint32_t** outStart, uint32_t* outEnd, Mode mode) {
    Result result = conversionOK;
    const uint8_t* in = *inStart;
    uint32_t* out = *outStart;
    while (in < inEnd) {
        uint32_t ch = 0;
        unsigned short extraBytesToRead = trailingBytesForUTF8[*in];
        if (extraBytesToRead >= inEnd - in) {
            result = inExhausted; break;
        }
        /* Do this check whether lenient or strict */
        if (!isLegalUTF8(in, extraBytesToRead+1)) {
            result = inIllegal;
            break;
        }
        /*
         * The cases all fall through. See "Note A" below.
         */
        switch (extraBytesToRead) {
            case 5: ch += *in++; ch <<= 6;
            case 4: ch += *in++; ch <<= 6;
            case 3: ch += *in++; ch <<= 6;
            case 2: ch += *in++; ch <<= 6;
            case 1: ch += *in++; ch <<= 6;
            case 0: ch += *in++;
        }
        ch -= offsetsFromUTF8[extraBytesToRead];

        if (out >= outEnd) {
            in -= (extraBytesToRead+1); /* Back up the in pointer! */
            result = outExhausted; break;
        }
        if (ch <= UNI_MAX_LEGAL_UTF32) {
            /*
             * UTF-16 surrogate values are illegal in UTF-32, and anything
             * over Plane 17 (> 0x10FFFF) is illegal.
             */
            if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
                if (mode == strictConversion) {
                    in -= (extraBytesToRead+1); /* return to the illegal value itself */
                    result = inIllegal;
                    break;
                } else {
                    *out++ = UNI_REPLACEMENT_CHAR;
                }
            } else {
                *out++ = ch;
            }
        } else { /* i.e., ch > UNI_MAX_LEGAL_UTF32 */
            result = inIllegal;
            *out++ = UNI_REPLACEMENT_CHAR;
        }
    }
    *inStart = in;
    *outStart = out;
    return result;
}

/* ---------------------------------------------------------------------
    Note A.
    The fall-through switches in UTF-8 reading code save a
    temp variable, some decrements & conditionals.  The switches
    are equivalent to the following loop:
        {
            int tmpBytesToRead = extraBytesToRead+1;
            do {
                ch += *in++;
                --tmpBytesToRead;
                if (tmpBytesToRead) ch <<= 6;
            } while (tmpBytesToRead > 0);
        }
    In UTF-8 writing code, the switches on "bytesToWrite" are
    similarly unrolled loops.

   --------------------------------------------------------------------- */
