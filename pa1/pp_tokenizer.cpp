#include "pp_tokenizer.h"
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <assert.h>
#include <stdlib.h>

/*
 * This preprocessing tokenizer is implemented to be conformant with the N4527
 * specification.  You may download a free copy of the N4527 from:
 *
 *      http://open-std.org/JTC1/SC22/WG21/docs/papers/2015/n4527.pdf
 */

/*
 * The order in which strings are initialized must be exactly the same as in he
 */
const std::vector<std::string> pp_tokenizer::_token_type_to_string_map = {
        "whitespace_sequence",
        "new_line",
        "header_name",
        "identifier",
        "pp_number",
        "character_literal",
        "user_defined_character_literal",
        "string_literal",
        "user_defined_string_literal",
        "preprocessing_op_or_punc",
        "non_whitespace_char",
        "eof",
        "num_token"
};

/*
 * 2.3 Character sets
 * 1. The basic source character set consists of 96 characters:
 *        space character,
 *        horizontal tab,
 *        vertical tab,
 *        from feed,
 *        new-line,
 *    and 91 more characters.
 * 2. Universal-character-name construct:
 *    hex-quad:
 *        hexadecimal-digit hexadecimal-digit hexadecimal-digit hexadecimal-digit
 *    universal-character-name:
 *        \u hex-quad
 *        \U hex-quad hex-quad
 *    - The character designated by the universal-character-name \UXXXXXXXX is
 *      that character whose character short name in ISO/IEC 10646 is NNNNNNNN.
 *    - Similarly, \uNNNN represents 0000NNNN.
 *    - If the hexadecimal value for a universal-character-name corresponds to a
 *      surrogate code point (0xD800 - 0xDFFF, inclusive), the program is
 *      ill-formed.
 *    - Additionally, if the hexadecimal value for a universal-character-name
 *      outside the c-char-sequence, s-char-sequence, or r-char-sequence of a
 *      character or string literal corresponds to a control character (0x00 -
 *      0x1F, 0x7F - 0x9F, both inclusive), or to a character in the basic
 *      source character set, the program is ill-formed.
 *    - A sequence of characters resembing a universal-character-name in an
 *      r-char-sequence (2.14.5) does not form a universal-character-name.
 */
const std::unordered_set<char> pp_tokenizer::_basic_source_character_set = {
        ' ',
        '\t',
        '\n',
        static_cast<char>(11), // vertical tab
        static_cast<char>(12), // form feed

        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',

        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',

        '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',

        ')', '{', '}', '[', ']', '#', '(', ')', '<', '>', '%', ':', ';',
        '.', '?', '*', '+', '-', '/', '^', '&', '|', '~', '!', '=', ',',
        '\\',
        '\"',
        '\''
};

/* 
 * 2.4 Trigraph sequences
 * NOTE: Trigraphs are removed in C++14
 * ??X => Y is represented as X => Y in this hash table.
 */
const std::unordered_map<char, char> pp_tokenizer::_trigraph_sequence_map = {
        std::pair<char, char>('=', '#'),
        std::pair<char, char>('/', '\\'),
        std::pair<char, char>('\'', '^'),

        std::pair<char, char>('(', '['),
        std::pair<char, char>(')', ']'),
        std::pair<char, char>('!', '|'),

        std::pair<char, char>('<', '{'),
        std::pair<char, char>('>', '}'),
        std::pair<char, char>('-', '~'),
};

/*
 * 2.4 Preprocessing tokens
 * preprocessing-token:
 *        header-name
 *        identifier
 *        pp-number
 *        character-literal
 *        user-defined-character-literal
 *        string-literal
 *        user-defined-string-literal
 *        preprocessing-op-or-punc
 *        each non-white-space character that cannot be one of the above
 */

/*
 * 2.5 Alternative tokens
 * 1. Alternative token representations are provided for some operators and
 *    punctuators.
 * 2. In all aspects of the language, each alternative token behaves the same,
 *    respectively, as its primary token, except for its spelling.
 */
const std::unordered_map<std::string, std::string>
pp_tokenizer::_alternative_token_map = {
        std::pair<std::string, std::string>("<%", "{"),
        std::pair<std::string, std::string>("%>", "}"),
        std::pair<std::string, std::string>("<:", "["),
        std::pair<std::string, std::string>(":>", "]"),
        std::pair<std::string, std::string>("%:", "#"),
        std::pair<std::string, std::string>("%:%:", "##"),

        std::pair<std::string, std::string>("and", "&&"),
        std::pair<std::string, std::string>("bitor", "|"),
        std::pair<std::string, std::string>("or", "||"),
        std::pair<std::string, std::string>("xor", "^"),
        std::pair<std::string, std::string>("compl", "~"),
        std::pair<std::string, std::string>("bitand", "&"),

        std::pair<std::string, std::string>("and_eq", "&="),
        std::pair<std::string, std::string>("or_eq", "|="),
        std::pair<std::string, std::string>("xor_eq", "^="),
        std::pair<std::string, std::string>("not", "!"),
        std::pair<std::string, std::string>("not_eq", "!="),
};

/*
 * 2.6 Tokens
 * token:
 *        identifier
 *        keyword
 *        literal
 *        operator
 *        punctuators
 *
 * There are five kinds of tokens. Blanks, horizontal and vertical tabs,
 * newlines, formfeeds, and comments, as described below, are collectively
 * "white spaces". "White spaces" are ignored except as they serve to separate
 * tokens. 
 */

/*
 * 2.7 Comments
 * The characters / * start a comment, which terminates with the characters * /.
 * These comments do not nest.
 * The characters // start a comment, which terminates with the next '\n'.
 * If there is a form-feed or a vertical-tab character in such a comment, only
 * white-space characters shall appear between it and the '\n' that terminates
 * the comment. No diagnostic is required.
 */

/*
 * 2.8 Header names
 * h-char; any member of the source character set except new-line and >
 * q-char; any member of the source character set except new-line and "
 */
const std::unordered_set<char> pp_tokenizer::_h_char_set = {
        ' ',
        '\t',
        //'\n',
        static_cast<char>(11), // vertical tab
        static_cast<char>(12), // form feed

        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',

        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',

        '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',

        ')', '{', '}', '[', ']', '#', '(', ')', '<',      '%', ':', ';',
        '.', '?', '*', '+', '-', '/', '^', '&', '|', '~', '!', '=', ',',
        '\\',
        '\"',
        '\''
};

const std::unordered_set<char> pp_tokenizer::_q_char_set = {
        ' ',
        '\t',
        //'\n',
        static_cast<char>(11), // vertical tab
        static_cast<char>(12), // form feed

        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',

        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',

        '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',

        ')', '{', '}', '[', ']', '#', '(', ')', '<', '>', '%', ':', ';',
        '.', '?', '*', '+', '-', '/', '^', '&', '|', '~', '!', '=', ',',
        '\\',
        //'\"',
        '\''
};

/*
 * 2.9 Preprocessing numbers
 * pp-numbers:
 *        digit
 *        . digit
 *        pp-number digit
 *        pp-number identifier-nondigit
 *        pp-number e sign
 *        pp-number E sign
 *        pp-number .
 * 1. Preprocessing number tokens lexically include all integral literal tokens
 *    (2.14.2) and all floating literal tokens (2.14.4).
 * 2. A preprocessing number does not have a type or a value. It acquires both
 *    after a successful converison to an integral literal token or a floating
 *    literal token.
 */

/*
 * 2.10 Identifiers
 * identifier:
 *        identifier-nondigit
 *        identifier identifier-nondigit
 *        identifier digit
 * identifier-nondigit:
 *        nondigit
 *        universal-character-name
 *        other implementation-defined characters
 * nondigit:
 *        a-z A-Z _
 * digit:
 *        0-9
 * 1. - An identifier is an arbitrarily long sequence of letters and digits.
 *    - Each universal-character-name in an identifier shall designate a
 *      character whose encoding in ISO 10646 falls into one of the ranges
 *      specified in E.1.
 *    - The initial element shall not be a universal-character-name designating
 *      a character whose encoding falls into the ranges specified in E.2.
 *    - Upper- and lower-case letters are different.
 *    - All characters are significant.
 * 2. - The identifiers in Table 3 have a special meaning when appearing in a
 *      certain context. When referred to in the grammar, these identifiers are
 *      used explicitly rather than using the identifier grammar production.
 *    - Any ambiguity as to whether a given identifier has a special meaning is
 *      resolved to interpret the token as a regular identifier.
 * Table 3:
 *        override
 *        final
 * 3. In addition, some identifiers are reserved for use by C++ implementations
 *    and standard libraries (17.6.4.3.2) and shall NOT be used otherwise. No
 *    diagnostic is required.
 */
const std::unordered_set<char> pp_tokenizer::_nondigit_set = {
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',

        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',

        '_'
};

const std::unordered_set<char> pp_tokenizer::_digit_set = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
};

/*
 * 2.11 Keywords
 * 1. - Keywords are special identifiers, as listed in Table 4.
 *    - Keywords are unconditionally treated as keywords in phase 7.
 *    - The export keyword is unused but is reserved for future use.
 * 2. Furthermore, the alternative representations shown in Table 5 for certain
 *    operators and punctuators (2.6) are reserved and shall not be used
 *    otherwise.
 * Table 5:
 *    and     and_eq  bitand  bitor  compl   not
 *    not_eq  or      or_eq   xor    xor_eq
 */

/*
 * Table 4:
 */
const std::unordered_set<std::string> pp_tokenizer::_keyword_set = {
        "alignas",
        "alignof",
        "asm",
        "auto",
        "bool",
        "break",
        "case",
        "catch",
        "char",
        "char16_t",
        "char32_t",
        "class",
        "const",
        "constexpr",
        "const_cast",

        "continue",
        "decltype",
        "default",
        "delete",
        "do",
        "double",
        "dynamic_cast",
        "else",
        "enum",
        "explicit",
        "export",
        "extern",
        "false",
        "float",
        "for",

        "friend",
        "goto",
        "if",
        "inline",
        "int",
        "long",
        "mutable",
        "namespace",
        "new",
        "noexcept",
        "nullptr",
        "operator",
        "private",
        "protected",
        "public",

        "register",
        "reinterpret_cast",
        "return",
        "short",
        "signed",
        "sizeof",
        "static",
        "static_assert",
        "static_cast",
        "struct",
        "switch",
        "template",
        "this",
        "thread_local",
        "throw",

        "true",
        "try",
        "typedef",
        "typeid",
        "typename",
        "union",
        "unsigned",
        "using",
        "virtual",
        "void",
        "volatile",
        "wchar_t",
        "while"
};

/*
 * 2.12 Operators and punctuators
 * Each preprocessing-op-or-punc token is converted to a single token in
 * translation phase 7 (2.2)
 */
//std::unordered_set<std::string> pp_tokenizer::_preprocessing_op_or_punc_set = {
//};

/*
 * 2.13.1 Kinds of literals
 * literal:
 *        integer-literal
 *        character-literal
 *        floating-literal
 *        string-literal
 *        boolean-literal
 *        pointer-literal
 *        user-defined-literal
 */

/*
 * 2.13.2 Integer literals
 * integer-literal:
 *        decimal-literal integer-suffix(optional)
 *        octal-literal integer-suffix(optional)
 *        hexadecimal-literal integer-suffix(optional)
 * decimal-literal:
 *        nonzero-digit
 *        decimal-literal digit
 * octal-literal:
 *        0
 *        octal-literal octal-digit
 * hexadecimal-literal:
 *        0x hexadecimal-digit
 *        0X hexadecimal-digit
 *        hexadecimal-literal hexadecimal-digit
 * nonzero-digit:
 *        1-9
 * octal-digit:
 *        0-8
 * hexadecimal-digit:
 *        0-9 a-f A-F
 * integer-suffix:
 *        unsigned-suffix long-suffix(optional)
 *        unsigned-suffix long-long-suffix(optional)
 *        long-suffix unsigned-suffix(optional)
 *        long-long-suffix unsigned-suffix(optional)
 * unsigned-suffix:
 *         u U
 * long-suffix:
 *        l L
 * long-long-suffix:
 *        ll LL
 *
 *
 * 2. The type of an integer literal is the first of the corresponding list in
 *    Table 6, in which its value can be represented.
 * Table 6 -- Types of integer constants
 * ----------------------------------------------------------------------------
 * | Suffix       |   Decimal constants       | Octal or hexadecimal constant |
 * ----------------------------------------------------------------------------
 * | none         |   int                     |  (unsigned) int               |
 * |              |   long int                |  (unsigned) long int          |
 * |              |   long long int           |  (unsigned) long long int     |
 * ----------------------------------------------------------------------------
 * | u or U       |   unsigned int            |   unsigned int                |
 * |              |   unsigned long int       |   unsigned long int           |
 * |              |   unsigned long long int  |   unsigned long long int      |
 * ----------------------------------------------------------------------------
 * | l or L       |   long int                |  (unsigned) long int          |
 * |              |   long long int           |  (unsigned) long long int     |
 * ----------------------------------------------------------------------------
 * | Both u or U  |   unsigned long int       |   unsigned long int           |
 * | and l or L   |   unsigned long long int  |   unsigned long long int      |
 * ----------------------------------------------------------------------------
 * | ll or LL     |   long long int           |  (unsigned) long long int     |
 * ----------------------------------------------------------------------------
 * | Both u and U |   unsigned long long int  |   unsigned long long int      |
 * | and ll or LL |                           |                               |
 * ----------------------------------------------------------------------------
 * 3. - If an integer literal cannot be represented by any type in its list,
 *      and an extended integer type (3.9.1) can represent its value, it may
 *      have that extended integer type.
 *    - If all of the types in the list for the literal are signed/unsigned,
 *      the extended integer type shall be signed/unsigned.
 *    - If the list contains both signed and unsigned types, the extended
 *      integer type may be signed or unsigned.
 *    - If one of its translation units contains an integer literal that cannot
 *      be represented by any of the allowed types, the program is ill-formed.
 */

/*
 * 2.13.3 Character literals
 * character-literal:
 *        ' c-char-sequence '
 *        u' c-char-sequence '
 *        U' c-char-sequence '
 *        L' c-char-sequence '
 * c-char-sequence:
 *        c-char
 *        c-char-sequence c-char
 * c-char:
 *        any member of the source character set except:
 *                singel-quote '
 *                backslash \,
 *                new-line '\n',
 *        escape-sequence
 *        univeral-character-name
 * escape-sequence:
 *        simple-escape-sequence
 *        octal-escape-sequence
 *        hexadecimal-escape-sequence
 * simple-escape-sequence:
 *        \'   \"   \?  \\
 *        \a   \b   \f  \n
 *        \r   \t   \v
 * octal-escape-sequence:
 *        \ octal-digit
 *        \ octal-digit octal-digit
 *        \ octal-digit octal-digit octal-digit
 * hexadecimal-escape-sequence:
 *        \x hexadecimal-digit
 *        hexadecimal-escape-sequence hexadecimal-digit
 * 1. - A character literal is on or more characters enclosed in single quotes,
 *      as in 'x', optionally preceded by one of the letters u, U, or L, as in
 *      u'y', U'z', or L'x', respectively.
 *    - A character literal that does not begin with u, U, or L is an ordinary
 *      character literal, also referred to as a narrow-character literal.
 *    - An ordinary character literal that contains a single c-char
 *      representable in the execution character set has type char, with value
 *      equal to the numerical value of the encoding of the c-char in the
 *      executaion character set.
 *    - An ordinary character literal that contains more than one c-char is a
 *      multicharacter literal.
 *    - A multicharacter literal, or an ordinary character literal containing a
 *      single c-char not representable in the executation character set, is
 *      conditionally supported, has type int, and has an implementation-defined
 *      value.
 * 2. - u'y' has type char16_t. If the unicode code point is not representable
 *      within 16 bits, the program is ill-formed.
 *    - A char16_t containing multiple c-chars is ill-formed.
 *    - U'z' has type char32_t.
 *    - A char32_t containing multiple c-chars is ill-formed.
 *    - L'x' has type wchar_t, or a wide-character literal.
 *    - The type wchar_t is able to represent all members of the executtion
 *      wide-character set (see 3.9.1).
 *    - The value of a wide-character literal containing multiple c-chars is
 *      implementation-defined.
 * 3. - Certain nongraphic characters, the single quote ', the double quote ",
 *      the question mark ?, and the backslash \, can be represented according
 *      Table 7.
 *    - The double quote " and the question mark ?, can be represented as
 *      themselves or by the escape sequences \" and \? respectively.
 *    - The single quote ' and the backslash \ can only be represented by the
 *      escape sequences \' and \\ respectively.
 *    - Escape sequences in which the character following the backslash is not
 *      listed in Table 7 are conditionally-supported, with
 *      implementation-defined semantics.
 *    - An escape sequence specifies a single character.
 * 4. - The escape \ooo consists of the backslash followed by one, two, or three
 *      octal digits that are taken to specify the value of the desired
 *      character.
 *    - The escape \xhhh consists of the backslash followed by x followed by one
 *      or more hexadecimal digits that are taken to specify the value of the
 *      desired character.
 *    - A sequence of octal or hexadecimal digits is terminated by the first
 *      character that is not an octal digit or a hexadecimal digit,
 *      respectively.
 *    - The value of a character literal is implementation-defined if it falls
 *      outside of the implementation-defined range defined for char, char16_t,
 *      char32_t, or wchar_t, respectively.
 * 5. - A universal-character-name is translated to the encoding, in the
 *      appropriate executation character set, of the character named.
 *    - If there is no such encoding, the universal
 */

/*
 * Table 7 -- Escape sequences
 * new-line         NL   \n
 * horizontal tab   HT   \t
 * vertical tab     VT   \v
 * backspace        BS   \b
 * carriage return  CR   \r
 * form feed        FF   \f
 * alert            BEL  \a
 * backslash        \    \\
 * question mark    ?    \?
 * single quote     '    \'
 * double quote     "    \"
 * octal number     ooo  \ooo
 * hex number       hhh  \xhhh
 */

std::vector<uint64_t> pp_tokenizer::_state_bit_mask_list(pp_tokenizer::NUM_STATES);

pp_tokenizer::pp_tokenizer(FILE *fin): _stream(fin)
{
        assert(NUM_STATES < 64);
        for (int i = 0; i < NUM_STATES; i++)
                _state_bit_mask_list[i] = 1 << i;
}

int pp_tokenizer::issue_token(FILE *fout)
{
        if (this->_type == new_line)
                fprintf(fout,"new_line 0\n");
        else if (this->_type == eof)
                fprintf(fout,"eof\n");
        else
                fprintf(fout,"%s %zu %s\n",
                                _token_type_to_string_map[this->_type].c_str(),
                                this->_data.size(),
                                this->_data.c_str());
        return 0;
}

//// Translation features you need to implement:
//// - utf8 decoder
//// - utf8 encoder
//// - universal-character-name decoder
//// - trigraphs
//// - line splicing
//// - newline at eof
//// - comment striping (can be part of whitespace-sequence)

//// EndOfFile: synthetic "character" to represent the end of source file
//constexpr int EndOfFile = -1;

//// given hex digit character c, return its value
//int HexCharToValue(int ch)
//{
        //switch (ch) {
        //case '0': return 0;
        //case '1': return 1;
        //case '2': return 2;
        //case '3': return 3;
        //case '4': return 4;
        //case '5': return 5;
        //case '6': return 6;
        //case '7': return 7;
        //case '8': return 8;
        //case '9': return 9;
        //case 'A': return 10;
        //case 'a': return 10;
        //case 'B': return 11;
        //case 'b': return 11;
        //case 'C': return 12;
        //case 'c': return 12;
        //case 'D': return 13;
        //case 'd': return 13;
        //case 'E': return 14;
        //case 'e': return 14;
        //case 'F': return 15;
        //case 'f': return 15;
        //default:
                  //fprintf(stderr,"Error trying to convert <%c> to integer. Abort.\n", (char)ch);
                  //abort();
        //}
//}

//// See C++ standard 2.11 Identifiers and Appendix/Annex E.1
//const std::vector<std::pair<int, int>> AnnexE1_Allowed_RangesSorted =
//{
        //{0xA8,0xA8},
        //{0xAA,0xAA},
        //{0xAD,0xAD},
        //{0xAF,0xAF},
        //{0xB2,0xB5},
        //{0xB7,0xBA},
        //{0xBC,0xBE},
        //{0xC0,0xD6},
        //{0xD8,0xF6},
        //{0xF8,0xFF},
        //{0x100,0x167F},
        //{0x1681,0x180D},
        //{0x180F,0x1FFF},
        //{0x200B,0x200D},
        //{0x202A,0x202E},
        //{0x203F,0x2040},
        //{0x2054,0x2054},
        //{0x2060,0x206F},
        //{0x2070,0x218F},
        //{0x2460,0x24FF},
        //{0x2776,0x2793},
        //{0x2C00,0x2DFF},
        //{0x2E80,0x2FFF},
        //{0x3004,0x3007},
        //{0x3021,0x302F},
        //{0x3031,0x303F},
        //{0x3040,0xD7FF},
        //{0xF900,0xFD3D},
        //{0xFD40,0xFDCF},
        //{0xFDF0,0xFE44},
        //{0xFE47,0xFFFD},
        //{0x10000,0x1FFFD},
        //{0x20000,0x2FFFD},
        //{0x30000,0x3FFFD},
        //{0x40000,0x4FFFD},
        //{0x50000,0x5FFFD},
        //{0x60000,0x6FFFD},
        //{0x70000,0x7FFFD},
        //{0x80000,0x8FFFD},
        //{0x90000,0x9FFFD},
        //{0xA0000,0xAFFFD},
        //{0xB0000,0xBFFFD},
        //{0xC0000,0xCFFFD},
        //{0xD0000,0xDFFFD},
        //{0xE0000,0xEFFFD}
//};

//// See C++ standard 2.11 Identifiers and Appendix/Annex E.2
//const std::vector<std::pair<int, int>> AnnexE2_DisallowedInitially_RangesSorted =
//{
        //{0x300,0x36F},
        //{0x1DC0,0x1DFF},
        //{0x20D0,0x20FF},
        //{0xFE20,0xFE2F}
//};

//// See C++ standard 2.13 Operators and punctuators
//const std::unordered_set<std::string> Digraph_IdentifierLike_Operators =
//{
        //"new", "delete", "and", "and_eq", "bitand",
        //"bitor", "compl", "not", "not_eq", "or",
        //"or_eq", "xor", "xor_eq"
//};

//// See `simple-escape-sequence` grammar
//const std::unordered_set<int> SimpleEscapeSequence_CodePoints =
//{
        //'\'', '"', '?', '\\', 'a', 'b', 'f', 'n', 'r', 't', 'v'
//};
