#include "PPCodePointCheck.h"
#include "PPCodeUnitCheck.h"
#include "PPTokenizerDFA.h"
#include <assert.h>

// TODO: Create a common utility debug logging tool instead of this macro
// solution.
// Comment the following line to see all sorts of debug information at run time.
#define fprintf(stderr,...)

PPTokenizerDFA::PPTokenizerDFA(std::shared_ptr<PPCodeUnitStreamIfc> stream):
  _stream(stream)
{
  _pushTokens();
}

bool PPTokenizerDFA::isEmpty() const
{
  return _stream->isEmpty()  &&  _queue.empty();
}

std::shared_ptr<PPToken> PPTokenizerDFA::getPPToken() const
{
  assert(!_queue.empty());
  return _queue.front();
}

void PPTokenizerDFA::toNext()
{
  assert(!_queue.empty());
  _queue.pop();
  if (_queue.empty())
    _pushTokens();
}

std::string PPTokenizerDFA::getErrorMessage() const{
  return _errorMessage;
}

void PPTokenizerDFA::_setError(const std::string &&msg)
{
  _errorMessage = std::move(msg);
}

void PPTokenizerDFA::_clearError()
{
  _errorMessage.clear();
}

void PPTokenizerDFA::_pushTokens()
{
  assert(_queue.empty());

  enum class State {
    Start = 0,

    HeaderNameH,    // e.g., <stdio.h>
    HeaderNameQ,    // e.g., "my_lib.h"

    PPNumber_LackDigit,
    PPNumber_Exponent,
    PPNumber_Apostrophe,
    PPNumber,

    Identifier,

    Slash,          // / // /*
    SingleLineComment,
    MultipleLineComment,
    MultipleLineCommentStar,

    EqualSignOp,    // e.g., {+ +=}, {- -=}

    VerticalBar,    // |
    PoundSign,      // #
    Ampersand,      // &
    Plus,           // +
    Minus,          // -
    Minus2,         // ->
    Divide,         // /
    Column,         // :
    PercentSign,    // %
    PercentSign2,   // %:%
    Dot,            // .
    DotDot,         // ..
    Bra,            // <
    BraBra,         // <<
    Ket,            // >
    KetKet,         // >>

    PossibleCharacterOrStringLiteral, // u8, u8, U, L
    PossibleRawStringLiteral, // u8R, R, u8R, UR, LR
    CharacterLiteral,
    CharacterLiteralEscape,
    CharacterLiteralHex,
    CharacterLiteralOct,
    CharacterLiteralEnd,

    UserDefinedCharacterLiteral,

    RawString,
    RawStringDelimiter,
    RawStringKet,

    StringLiteral,
    StringLiteralEscape,
    StringLiteralHex,
    StringLiteralOct,
    StringLiteralEnd,

    UserDefinedStringLiteral,


    NumberOfStates, // Convenience for statistics

    End,
    Error
  } state = State::Start;

  // String variables used by the DFA.
  std::string comment_u8str;
  std::string header_name_u8str;
  std::string ppnumber_u8str;
  std::string equal_sign_op_u8str;
  std::string identifier_u8str;
  std::string encoding_prefix_u8str;
  std::string character_literal_u8str;
  std::string string_literal_u8str;
  std::string raw_string_u8str;
  std::string raw_string_delimiter_u8str;
  std::string raw_string_ket_u8str;
  std::string ud_suffix_u8str;
  std::string oct_escape_u8str;

  const auto _emitToken = [this] (const std::shared_ptr<PPToken> tok) {
    fprintf(stderr,"======== %s =======\n", tok->getUTF8String().c_str());
    this->_queue.push(tok);
  };

  const auto _toNext = [this] () {
    char32_t tmp;
    tmp = _stream->getCodeUnit()->getChar32();
    fprintf(stderr,"%c(%0X) => ", tmp, tmp);
    this->_stream->toNext();
    if (!this->_stream->isEmpty()) {
      tmp = _stream->getCodeUnit()->getChar32();
      fprintf(stderr,"%c(%0X)\n", tmp, tmp);
    }
  };

  while (!_stream->isEmpty()  &&  state != State::End  &&  state != State::Error) {
    // In a state block, e.g., the block for if (state == State::SomeState), the
    // developer needs to call _stream->toNext() explicitly otherwise the stream
    // PPCodeUnitStream object will NOT move forward.
    //
    // The reason for doing this is to allow a state to not consume the symbol
    // being processed, e.g., in State::LeftParenthesis.
    const std::shared_ptr<PPCodeUnit> curr = _stream->getCodeUnit();
    const char32_t currChar32 = curr->getChar32();
    fprintf(stderr,"\n==  U+%06X <%c> \n", static_cast<uint32_t>(currChar32), static_cast<char>(currChar32));

    if (state == State::Start) {
      // State::Start means starting to parse and emit the next PPToken.
      // Specifically, this does not imply start of line/file, although in
      // certain cases it could be a start of line/file.
      fprintf(stderr,"State::Start\n");
      _toNext();

      if (currChar32 == U'\n') {
        // The begining of a line is either immediately after a newline
        // character \n or is at the start of the input stream.
        state = State::End;
        _emitToken(PPToken::createNewLine());
        _isBeginningOfLine = true;
        _isBeginningOfHeaderName = false;
      }

      // simple-op-or-punc
      else if (currChar32 == U'{'  ||  currChar32 == U'}'  ||  currChar32 == U'['
          ||   currChar32 == U']'  ||  currChar32 == U'('  ||  currChar32 == U')'
          ||   currChar32 == U'?'  ||  currChar32 == U','  ||  currChar32 == U';'
          ) {
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc(std::string(1, static_cast<char>(currChar32))));
      }

      // The slash is the common entry point for three different states:
      //    preprocessing-op-or-punc /
      //    comments: // /*
      else if (currChar32 == U'/') {
        state = State::Slash;
      }

      // equal-sign-punc
      else if (currChar32 == U'*'  ||  currChar32 == U'/'  ||  currChar32 == U'^'
          ||   currChar32 == U'~'  ||  currChar32 == U'!'  ||  currChar32 == U'=') {
        state = State::EqualSignOp;
        equal_sign_op_u8str = std::string(1, static_cast<char>(currChar32));
      }

      // stateful-op-or-punc
      // header-name h-sequence, e.g., <stdio.h> enters here too
      else if (currChar32 == U'#') { // # ##
        state = State::PoundSign;
      } else if (currChar32 == U'|') { // | |= ||
        state = State::VerticalBar;
      } else if (currChar32 == U'+') { // + += ++
        state = State::Plus;
      } else if (currChar32 == U'-') { // - -= -- -> ->*
        state = State::Minus;
      } else if (currChar32 == U'&') { // & &= &&
        state = State::Ampersand;
      } else if (currChar32 == U'<') { // < <= <% <: << <<= HeaderName_HSequenceStart
        state = _isBeginningOfHeaderName ? State::HeaderNameH : State::Bra;
        header_name_u8str.clear();
      } else if (currChar32 == U'>') { // > >= >> >>=
        state = State::Ket;
      } else if (currChar32 == U':') { // : :> ::
        state = State::Column;
      } else if (currChar32 == U'%') { // % %> %: %:%:
        state = State::PercentSign;
      } else if (currChar32 == U'.') { // . .* ... PPNumber_LackDigit
        state = State::Dot;
      }

      else if (PPCodeUnitCheck::isIdentifierStart(curr)) {
        fprintf(stderr,"isIdentifierStart %c\n", static_cast<char>(currChar32));
        identifier_u8str = curr->getUTF8String();
        state = State::Identifier;
      }

      // ordinary string-literal, a string-literal without any encoding prefix.
      // header-name q-sequence, e.g., "my_lib.h" enters here too
      else if (currChar32 == U'\"') {
        if (_isBeginningOfHeaderName) {
          state = State::HeaderNameQ;
          header_name_u8str.clear();
        } else {
          state = State::StringLiteral;
          string_literal_u8str = "\"";
        }
      }

      else if (currChar32 == U'\'') {
        state = State::CharacterLiteral;
        character_literal_u8str = "\'";
      }


      else if (PPCodePointCheck::isDigit(currChar32)) {
        ppnumber_u8str = static_cast<char>(currChar32);
        state = State::PPNumber;
      }

      else if (currChar32 == U' ') {
        // no-op pass
      }

      else if (!PPCodePointCheck::isBasicSourceCharacter(currChar32)) {
        // TODO: Should probably emit some warnings here
        state = State::End;
        _emitToken(PPToken::createNonWhitespaceChar(std::u32string(1, currChar32)));
      }

      else {
        fprintf(stderr,"Dude, you literally exhausted all cases in the pptokenizer's DFA. The DFA does not know what you want to do here\n");
      }

    } // State::Start


    ////////////////////////////////////////////////////////////////////////////////
    // string-literal
    // user-defined-string-literal
    // character-literal
    // user-defined-character-literal
    //
    // Entry:
    //
    //   A string literal starts with either a double quote ", or an encoding
    //   prefix followed by a double quote ". If the encoding prefix ends with
    //   the letter R, the string literal is a raw string literal.
    //
    //   User defined literals are extensions of string-literal and
    //   character-literal with certain restrictions.
    //
    // Implementation:
    //
    //   We use UTF8 encoding for internal storage of string literals,
    //   regardless of their intended types. We convert them to the proper
    //   encoding in later phases.
    //
    //   In raw strings, universal-character-names are passed onto the next
    //   phase as-is. Otherwise, universal-character-names are replaced by their
    //   corresponding code ponints in the proper encoding.
    //
    ////////////////////////////////////////////////////////////////////////////////

    else if (state == State::PossibleCharacterOrStringLiteral) {
      // Previous: one of {u8, u, U, L}
      // '      =>  CharacterLiteral
      // "      =>  StringLiteral
      // other  =>  Emit identifier_u8str as a PPTokenIdentifier, curr
      //            PPCodeUnit is not consumed.
      //
      // Note:  raw-string and non-raw-string both use string_literal_u8str.
      // This is safe because at most one of the two can be true at a time.
      fprintf(stderr,"State::PossibleCharacterOrStringLiteral\n");

      if (currChar32 == U'\'') {
        _toNext();
        state = State::CharacterLiteral;
        character_literal_u8str = encoding_prefix_u8str + static_cast<char>(currChar32);
      } else if (currChar32 == U'\"') {
        _toNext();
        state = State::StringLiteral;
        string_literal_u8str = encoding_prefix_u8str + static_cast<char>(currChar32);
      } else {
        state = State::End;
        _emitToken(PPToken::createIdentifier(identifier_u8str));
      }
    }

    else if (state == State::PossibleRawStringLiteral) {
      // Previous: one of {u8R, uR, UR, LR, R}
      // "      =>  RawStringDelimiter, clear raw_string_delimiter_u8str.
      // other  =>  Emit identifier_u8str as a PPTokenIdentifier, curr
      //            PPCodeUnit is not consumed.

      if (currChar32 == U'\"') {
        _toNext();
        state = State::RawStringDelimiter;
        string_literal_u8str = encoding_prefix_u8str + static_cast<char>(currChar32);
        raw_string_delimiter_u8str.clear();
      } else {
        state = State::End;
        _emitToken(PPToken::createIdentifier(identifier_u8str));
      }
    }

    else if (state == State::CharacterLiteral) {
      // Previous: Start ', or PossibleCharacterOrStringLiteral '
      // c-char =>  Append curr->getUTF8String() to character_literal_u8str
      // '      =>  CharacterLiteralEnd
      // \      =>  CharacterLiteralEscape
      // other  =>  Error, curr PPCodeUnit is not consumed.
      //
      // Note: The C++ standard says that multicharacter literals are
      // conditionally supported, have type int, and have implementation defined
      // values. This tokenizer supports multicharacter literal but the value of
      // the literal is not determined by the tokenizer.
      fprintf(stderr,"State::CharacterLiteral\n");
      if (!PPCodeUnitCheck::isNotCChar(curr)) {
        _toNext();
        character_literal_u8str += curr->getUTF8String();
      } else if (currChar32 == U'\'') {
        _toNext();
        state = State::CharacterLiteralEnd;
        character_literal_u8str += static_cast<char>(currChar32);
      } else if (currChar32 == U'\\') {
        _toNext();
        state = State::CharacterLiteralEscape;
        character_literal_u8str += static_cast<char>(currChar32);
      } else {
        state = State::Error;
        _setError(R"(Expect a c-char, ', or \ in parsing character-literal.)");
      }
    }

    else if (state == State::CharacterLiteralEnd) {
      // Previous: '
      // identifier-start => UserDefinedCharacterLiteral
      // other            => Emit character_literal_u8str, curr PPCodeUnit is
      //                     not consumed.
      if (PPCodeUnitCheck::isIdentifierStart(curr)) {
        _toNext();
        state = State::UserDefinedCharacterLiteral;
        ud_suffix_u8str = std::string(1, static_cast<char>(currChar32));
      } else {
        state = State::End;
        _emitToken(PPToken::createCharacterLiteral(character_literal_u8str));
      }
    }

    else if (state == State::UserDefinedCharacterLiteral) {
      // Previous: CharacterLiteralEnd identifier-start
      // identifier-nonstart => UserDefinedCharacterLiteral
      // other               => Emit character_literal_u8str + ud_suffix_u8str as
      //                        user-defined-character-literal, curr PPCodeUnit
      //                        is not consumed.
      if (PPCodeUnitCheck::isIdentifierNonStart(curr)) {
        _toNext();
        ud_suffix_u8str += curr->getUTF8String();
      } else {
        state = State::End;
        _emitToken(PPToken::createUserDefinedCharacterLiteral(character_literal_u8str + ud_suffix_u8str));
      }
    }

    else if (state == State::CharacterLiteralEscape) {
      // Previous: CharacterLiteral \
      // ' " ? \ a b f n r t v  => CharacterLiteral
      // 0-7    =>  CharacterLiteralOct
      // x      =>  CharacterLiteralHex
      // other  =>  Error
      fprintf(stderr,"State::CharacterLiteralEscape\n");
      if (PPCodePointCheck::isSimpleEscapeChar(currChar32)) {
        _toNext();
        state = State::CharacterLiteral;
        character_literal_u8str += static_cast<char>(currChar32);
      } else if (PPCodePointCheck::isOctalDigit(currChar32)) {
        _toNext();
        state = State::CharacterLiteralOct;
        oct_escape_u8str = std::string(1, static_cast<char>(currChar32));
      } else if (currChar32 == U'x') {
        _toNext();
        state = State::CharacterLiteralHex;
        character_literal_u8str += static_cast<char>(currChar32);
      } else {
        state = State::Error;
        _setError(R"(Invalid escape sequence in parsing character-literal.)");
      }
    }

    else if (state == State::CharacterLiteralOct) {
      // Previous: CharacterLiteralEscape octal-digit
      // 0-7  and  oct_escape_u8str.length() < 3:
      //            Append currChar32 to oct_escape_u8str
      // other  =>  CharacterLiteral, append oct_escape_u8str to
      //            character_literal_u8str, curr PPCodeUnit is not consumed.
      //
      // Note: octal-escape-sequence can only take one of the following three
      // formats:
      //        \o \oo \ooo
      if (PPCodePointCheck::isOctalDigit(currChar32) && oct_escape_u8str.length() < 3) {
        _toNext();
        oct_escape_u8str += static_cast<char>(currChar32);
      } else {
        state = State::CharacterLiteral;
        character_literal_u8str += oct_escape_u8str;
      }
    }

    else if (state == State::CharacterLiteralHex) {
      // Previous: CharacterLiteralEscape, CharacterLiteralHex
      // hexadecimal-digit  =>  CharacterLiteralHex
      // other  => CharacterLiteral
      fprintf(stderr,"State::CharacterLiteralHex\n");
      if (PPCodePointCheck::isHexadecimalDigit(currChar32)) {
        _toNext();
        character_literal_u8str += static_cast<char>(currChar32);
      } else {
        state = State::CharacterLiteral;
      }
    }

    else if (state == State::StringLiteral) {
      // Previous: Start ", or PossibleCharacterOrStringLiteral "
      // "      => StringLiteralEnd, append " to string_literal_u8str.
      // \      => StringLiteralEscape
      // s-char => Append curr->getUTF8String() to string_literal_u8str.
      // other  => Error
      fprintf(stderr,"State::StringLiteral\n");
      if (currChar32 == U'\"') {
        _toNext();
        state = State::StringLiteralEnd;
        string_literal_u8str += static_cast<char>(currChar32);
      } else if (currChar32 == U'\\') {
        _toNext();
        state = State::StringLiteralEscape;
        string_literal_u8str += static_cast<char>(currChar32);
      } else if (!PPCodeUnitCheck::isNotSChar(curr)) {
        _toNext();
        string_literal_u8str += curr->getUTF8String();
      } else {
        state = State::Error;
        _setError(R"(Expect a quote ", backslash \, or an s-char to continue parsing string literal.)");
      }
    }

    else if (state == State::StringLiteralEscape) {
      // Previous: StringLiteral \
      // n t v b r f a \ ? ' " => StringLiteral
      // x      => StringLiteralHex
      // 0-7    => StringLiteralOct
      // other  => Error
      fprintf(stderr,"State::StringLiteralEscape\n");
      if (PPCodePointCheck::isSimpleEscapeChar(currChar32)) {
        _toNext();
        state = State::StringLiteral;
        string_literal_u8str += static_cast<char>(currChar32);
      } else if (currChar32 == U'x') {
        _toNext();
        state = State::StringLiteralHex;
        string_literal_u8str += static_cast<char>(currChar32);
      } else if (PPCodePointCheck::isOctalDigit(currChar32)) {
        _toNext();
        state = State::StringLiteralOct;
        oct_escape_u8str = std::string(1, static_cast<char>(currChar32));
      } else {
        state = State::Error;
        _setError(R"(Invalid character following \ in string-literal.)");
      }
    }

    else if (state == State::StringLiteralHex) {
      // Previous: StringLiteralEscape x, or StringLiteralHex hexadecimal-digit
      // hexadecimal-digit => StringLiteralHex, append currChar32 to
      //                      string_literal_u8str.
      // other             => StringLiteral, curr PPCodeUnit is not consumed.
      //
      // Note: Per the N4527 specification 2.13.3, hexadecimal escape sequence
      // can be arbitrarily long and terminates with the first non-hexadecimal-
      // digit character. For example:
      //     "\x9f3aff" = \9f3aff \0
      // An important implication of this parsing rule is that it is impossible
      // to describe using this language a single string with the following
      // memory image
      //    \9fffff 'f' \0,
      // because the second 'f' is unconditionally assimilated into the
      // preceding hexadeciaml escape sequence. Though, we can still achieve the
      // goal by concatenation:
      //    "\x9ffff" "f"
      //
      // In reality, Unicode code points can have at most six hexadecimal
      // digits. It might be advisable to stipulate that a hexadecimal escape
      // sequence in string literals have a maximal lengths of six hexadecimal
      // digits.
      if (PPCodePointCheck::isHexadecimalDigit(currChar32)) {
        _toNext();
        string_literal_u8str += static_cast<char>(currChar32);
      } else {
        state = State::StringLiteral;
      }
    }

    else if (state == State::StringLiteralOct) {
      // Previous: StringLiteral 0-7, or StringLiteralOct 0-7
      // octal-digit  &&  oct_escape_u8str.length() < 3:
      //           Append currChar32 to oct_escape_u8str.
      // other  => StringLiteral, curr PPCodeUnit is not consumed.
      //
      // Note: Per the N4527 specification 2.13.3, octal escape sequence has a
      // maximal length of three octal-digits or terminate with the first
      // non-octal-digit character, whichever comes first. For example:
      //     "\277"  = \277 \0
      //     "\0277" = \27 '7' \0
      if (PPCodePointCheck::isOctalDigit(currChar32) && oct_escape_u8str.length() < 3) {
        _toNext();
        oct_escape_u8str += static_cast<char>(currChar32);
      } else {
        state = State::StringLiteral;
        string_literal_u8str += oct_escape_u8str;
      }
    }

    else if (state == State::RawStringDelimiter) {
      // Previous: PossibleRawStringLiteral or RawStringDelimiter
      // (      => RawString, clear raw_string_u8str.
      //           The variable raw_string_delimiter_u8str stores the delimiter
      //           used in this raw string. It is possible that the delimiter is
      //           an empty string.
      // d-char => Append currChar32 to raw_string_delimiter_u8str
      // other  => Error
      if (currChar32 == U'(') {
        _toNext();
        state = State::RawString;
        raw_string_u8str.clear();
      } else if (!PPCodePointCheck::isNotDChar(currChar32)) {
        _toNext();
        raw_string_delimiter_u8str += static_cast<char>(currChar32);
      } else {
        state = State::Error;
        _setError(R"(Expect a ( or a d-char in parsing the delimiter d-sequence in raw string.)");
      }
    }

    else if (state == State::RawString) {
      // Previous: RawString or RawStringDelimiter
      // )      => RawStringKet, clear raw_string_ket_u8str
      // r-char (excluding ))
      //        => Append curr->getUTF8String() to raw_string_u8str
      // other  => Error
      //
      // Note: By definition, r-char is contextual, as in
      //    any member of the source character set, except a right parenthesis )
      //    followed by the initial d-char-sequence (which may be empty)
      //    followed by a double quote ".
      //
      //    As a result, if PPCodeUnitCheck::isNotRChar() returns false, the
      // char is _not_ guaranteed to be an r-char. The contextual informaion
      // needed to fully determin whether a PPCodeUnit is an r-char is stored in
      // raw_string_delimiter_u8str. The state RawStringKet is the state devoted
      // to determining r-char and end-of-string delimiters in raw strings.

      _toNext();
      if (currChar32 == U')') {
        state = State::RawStringKet;
        raw_string_ket_u8str.clear();
      } else if (!PPCodeUnitCheck::isNotRChar(curr)) {
        // Must be an r-char here. Note that universal-character-names shall be
        // reverted here using the getUTF8String() methods.
        raw_string_u8str += curr->getUTF8String();
      } else {
        state = State::Error;
        _setError(R"(Expecting an r-char in raw-string)");
      }
    }

    else if (state == State::RawStringKet) {
      // Previous: RawString or RawStringKet
      // "      => If raw_string_ket_u8str == raw_string_delimiter_u8str
      //           construct string_literal_u8str and transition to
      //           StringLiteralEnd, otherwise append raw_string_ket_u8str and "
      //           to raw_string_u8str and transition to RawString.
      // )      => Append raw_string_ket_u8str and ) to raw_string_u8str. Clear
      //           raw_string_ket_u8str.
      // d-char (excluding ")
      //        => Append currChar32 to raw_string_ket_u8str.
      // r-char (excluding d-char and ")
      //        => Append raw_string_ket_u8str and curr->getUTF8String() to
      //           raw_string_ket_u8str. Transition to RawString.
      // other  => Error

      _toNext();
      if (currChar32 == U'\"') {
        if (raw_string_ket_u8str == raw_string_delimiter_u8str) {
          state = State::StringLiteralEnd;
          string_literal_u8str = encoding_prefix_u8str +
            "\"" + raw_string_delimiter_u8str +
            "(" + raw_string_u8str + ")" +
            raw_string_delimiter_u8str + "\"";
        } else {
          raw_string_u8str += raw_string_ket_u8str;
          raw_string_u8str += static_cast<char>(currChar32);
        }
      } else if (currChar32 == U')') {
        raw_string_u8str += raw_string_ket_u8str;
        raw_string_u8str += static_cast<char>(currChar32);
        raw_string_ket_u8str.clear();
      } else if (!PPCodeUnitCheck::isNotDChar(curr)) {
        raw_string_ket_u8str += static_cast<char>(currChar32);
      } else if (!PPCodeUnitCheck::isNotRChar(curr)) {
        state = State::RawString;
        raw_string_u8str += raw_string_ket_u8str + curr->getUTF8String();
      } else {
        state = State::Error;
        _setError(R"(Expect a d-char, ", or an r-char in parsing a raw string.)");
      }
    }

    else if (state == State::StringLiteralEnd) {
      // Hereby, string_literal_u8str stores the longest input text that is
      // considered a string-literal for preprocessing lexing purpose.
      //
      // identifier-start => UserDefinedStringLiteral
      // other            => Emit string_literal_u8str as string-literal, curr
      //                     PPCodeUnit is not consumed.
      if (PPCodeUnitCheck::isIdentifierStart(curr)) {
        _toNext();
        state = State::UserDefinedStringLiteral;
        ud_suffix_u8str = std::string(1, static_cast<char>(currChar32));
      } else {
        state = State::End;
        _emitToken(PPToken::createStringLiteral(string_literal_u8str));
      }
    }

    else if (state == State::UserDefinedStringLiteral) {
      // Previous: StringLiteralEnd identifier-start
      // identifier-nonstart => UserDefinedStringLiteral
      // other               => Emit string_literal_u8str + ud_suffix_u8str as
      //                        user-defined-string-literal, curr PPCodeUnit is
      //                        not consumed.
      if (PPCodeUnitCheck::isIdentifierNonStart(curr)) {
        _toNext();
        ud_suffix_u8str += static_cast<char>(currChar32);
      } else {
        state = State::End;
        _emitToken(PPToken::createUserDefinedStringLiteral(string_literal_u8str + ud_suffix_u8str));
      }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // identifier
    //
    // Entry:
    //
    //   A nondigit, or a universal-character-name that is in AnnexE1 but not in
    //   AnnexE2.
    //
    //   The above description is different from but equivalent to the one in
    //   the C++ standard, N4527.
    //
    // Implentation notes:
    //   These two functions are desgined to make parsing identifiers trivial:
    //        PPCodeUnitCheck::isIdentifierStart()
    //        PPCodeUnitCheck::isIdentifierNonStart()
    //
    // CAVEAT (not handled here):
    //
    //   These two identifiers have special meanings: override, final
    //
    //   Some identifiers are reserved for C++ implementations and shall not be
    //   used. No diagnostic is required though. For complete listings, see
    //   N4527 section 2.11 (Keywords) Table 3 (keywords) and Table 4
    //   (identifier-like operators or punctuations), plus new and delete.
    //
    //   Identifiers that contains a double underscore __, or begin with an
    //   underscore _ followed by an uppercase letter are reserved to the
    //   implementation for any use. For example:
    //       __this_is_reserved
    //       this_Is_Also_Reserved
    //
    //   Identifiers that begin with an underscore _ are reserved to the
    //   implementation for use in the global namespace.
    //       _this_is_reserved_too
    //
    ////////////////////////////////////////////////////////////////////////////////

    else if (state == State::Identifier) {
      // Previous: identifier-start
      // identifier-nonstart  =>  Identifier
      // other                =>  Emit identifier or preprocessing-op-or-punc,
      //
      // When emitting:
      //
      //  - If _isBeginningOfLine is true and the identifier is "include", set
      //    _isPreprocessingDirective to true.
      //
      //  - If the identifier is "include" but _isBeginningOfLine is false,
      //    report error.
      //
      //  - If the identifier is one of the alternative representations:
      //        new delete and and_eq bitand bitor compl not not_eq or or_eq xor
      //        xor_eq,
      //    emit a preprocessing-op-or-punc. The string stored in the PPToken
      //    object is the alternative representation, not the primary
      //    representation.
      //
      //    Note that new and delete are not indeed alternative representations,
      //    but they are preprocessing-op-or-punc as well.
      //
      //  - If the identifier being emitted is one of the encoding prefixes:
      //        u8, u, U, L,
      //    transition to State::PossibleCharacterOrStringLiteral to check for
      //    either the single quote ' for starting a character literal or the
      //    double quote " for starting a non-raw string-literal.
      //
      //    Note that none of u8, u, U, and L is a reserved identifier. They can
      //    still be used as macros, variable names, or function names other
      //    than as encoding prefixes. This is why the processing of these
      //    identifier has to overlap with the identifier.
      //
      //  - If the identifier beining emitted is one of:
      //        u8R, uR, UR, LR, R
      //    transition to State::PossibleRawStringLiteral to check for the
      //    double quote ", an optinal d-sequence, and a left parenthesis for
      //    starting a raw-string-literal.
      //
      fprintf(stderr,"State::Identifier\n");

      fprintf(stderr,"%s + <%c> (U+%06X)\n", identifier_u8str.c_str(),
          static_cast<char>(currChar32), static_cast<uint32_t>(currChar32));

      const static std::vector<std::string> _ar_ = {
        "new", "delete", "and", "and_eq", "bitand", "bitor", "compl", "not",
        "not_eq", "or", "or_eq", "xor", "xor_eq"
      }; // ar is short for alternative representations

      if (PPCodeUnitCheck::isIdentifierNonStart(curr)) {
        _toNext();
        identifier_u8str += curr->getUTF8String();
        continue;
      } else if (std::find(_ar_.begin(), _ar_.end(), identifier_u8str) != _ar_.end()) {
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc(identifier_u8str));
      } else if (identifier_u8str == "include") {
        if (_isBeginningOfLine) {
          state = State::End;
          _emitToken(PPToken::createIdentifier(identifier_u8str));
          _isBeginningOfHeaderName = true;
        } else {
          state = State::Error;
          _setError(R"(The identifier "include" can only be used following # at the beginning of a new line)");
        }
      } else if (identifier_u8str == "u"  ||  identifier_u8str == "u8"
              || identifier_u8str == "U"  ||  identifier_u8str == "L") {
        state = State::PossibleCharacterOrStringLiteral;
        encoding_prefix_u8str = identifier_u8str;
      } else if (identifier_u8str == "uR"  ||  identifier_u8str == "u8R"
              || identifier_u8str == "UR"  ||  identifier_u8str == "LR"
              || identifier_u8str == "R") {
        state = State::PossibleRawStringLiteral;
        encoding_prefix_u8str = identifier_u8str;
      } else {
        state = State::End;
        _emitToken(PPToken::createIdentifier(identifier_u8str));
      }

      fprintf(stderr,"return from State::Identifier\n");
    }

    ////////////////////////////////////////////////////////////////////////////////
    // header-name
    //
    // Entry:
    //   The #include preprocessing directive ensuing a fresh new-line
    ////////////////////////////////////////////////////////////////////////////////

    else if (state == State::HeaderNameH) {
      // Previous: <
      // Set _isBeginningOfHeaderName to false.
      // h-char     =>  Append curr char to header_name_u8str.
      // >          =>  Emit header-name
      // other      =>  Error, curr PPCodeUnit is not consumed.
      fprintf(stderr,"State::HeaderNameH\n");

      _isBeginningOfHeaderName = false;
      if (currChar32 == U'>') {
        _toNext();
        state = State::End;
        header_name_u8str.insert(0, 1, '<');
        header_name_u8str += '>';
        _emitToken(PPToken::createHeaderName(header_name_u8str));
      } else if (PPCodePointCheck::isNotHChar(currChar32)) {
        state = State::Error;
        _setError(R"(Expecting an initial h-char for the header name.)");
      } else {
        _toNext();
        header_name_u8str += static_cast<char>(currChar32);
      }
    }

    else if (state == State::HeaderNameQ) {
      // Previous: "
      // Set _isBeginningOfHeaderName to false.
      // q-char     =>  Append curr char to header_name_u8str.
      // "          =>  Emit header-name.
      // other      =>  Error, curr PPCodeUnit is not consumed.
      fprintf(stderr,"State::HeaderNameQ\n");

      _isBeginningOfHeaderName = false;
      if (currChar32 == U'\"') {
        _toNext();
        state = State::End;
        header_name_u8str.insert(0, 1, '\"');
        header_name_u8str += '\"';
        _emitToken(PPToken::createHeaderName(header_name_u8str));
      } else if (PPCodePointCheck::isNotQChar(currChar32)) {
        state = State::Error;
        _setError(R"(Expecting a q-char for the header name.)");
      } else {
        _toNext();
        header_name_u8str += static_cast<char>(currChar32);
      }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // pp-number
    //
    // Entry:
    //   .      =>  PPNumber_LackDigit
    //   digit  =>  PPNumber
    ////////////////////////////////////////////////////////////////////////////////

    else if (state == State::PPNumber) {
      // Expect one of the following:
      //   ., digit, identifier-nondigit   =>  PPNumber
      //   '      =>  PPNumber_Apostrophe
      //   e, E   =>  PPNumber_Exponent
      //   other  =>  Emit pp-number, curr PPCodeUnit is not consumed.
      fprintf(stderr,"State::PPNumber\n");

      if (currChar32 == U'e'  ||  currChar32 == U'E') {
        _toNext();
        state = State::PPNumber_Exponent;
        ppnumber_u8str += static_cast<char>(currChar32);
      } else if (currChar32 == U'\'') {
        _toNext();
        state = State::PPNumber_Apostrophe;
        ppnumber_u8str += static_cast<char>(currChar32);
      } else if (currChar32 == U'.'  ||  PPCodePointCheck::isDigit(currChar32)  ||  PPCodeUnitCheck::isIdentifierNondigit(curr)) {
        _toNext();
        ppnumber_u8str += static_cast<char>(currChar32);
      } else {
        state = State::End;
        _emitToken(PPToken::createPPNumber(ppnumber_u8str));
      }
    }

    else if (state == State::PPNumber_LackDigit) {
      // Previous: dot .
      // digit  =>  PPNumber
      // other  =>  Error
      fprintf(stderr,"State::PPNumber_LackDigit\n");
      _toNext();

      if (PPCodePointCheck::isDigit(currChar32)) {
        state = State::PPNumber;
        ppnumber_u8str += static_cast<char>(currChar32);
      } else {
        state = State::Error;
        _setError(R"(Expecting a digit to form a pp-number)");
      }
    }

    else if (state == State::PPNumber_Exponent) {
      // Previous: e E
      // + -    =>  PPNumber
      // other  =>  Error
      _toNext();
      fprintf(stderr,"State::PPNumber_Exponent\n");

      if (PPCodePointCheck::isSign(currChar32)) {
        state = State::PPNumber;
        ppnumber_u8str += static_cast<char>(currChar32);
      } else {
        state = State::Error;
        _setError(R"(Expect a sign, i.e., either + or -, to transition to pp-number.)");
      }
    }

    else if (state == State::PPNumber_Apostrophe) {
      // Previous: '
      // digit, nondigit => PPNumber
      // other           => Error
      _toNext();
      fprintf(stderr,"State::PPNumber_Apostrophe\n");

      if (PPCodePointCheck::isDigit(currChar32) || PPCodePointCheck::isNondigit(currChar32)) {
        state = State::PPNumber;
        ppnumber_u8str += static_cast<char>(currChar32);
      } else {
        state = State::Error;
        _setError(R"(Expects a digit or a nondigit after an apostrophe)");
      }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // comment
    //
    // single-line-comment starts with double slash // and ends with a newline
    // (exclusive).
    //
    // multiple-line-comment starts with a /* and ends with */.
    //
    ////////////////////////////////////////////////////////////////////////////////

    else if (state == State::Slash) {
      // Previous: /
      // /      =>  SingleLineComment
      // *      =>  MultipleLineComment
      // other  =>  Emit /, curr PPCodeUnit is not consumed.
      fprintf(stderr,"State::Slash\n");

      if (currChar32 == U'/') {
        _toNext();
        state = State::SingleLineComment;
      } else if (currChar32 == U'*') {
        _toNext();
        state = State::MultipleLineComment;
      } else {
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc("/"));
      }
    }

    else if (state == State::SingleLineComment) {
      // Previous: //
      // \n     =>  Prepend "//' to comment_u8str, emit comment_u8str as
      //            whitespace-sequence
      // other  =>  SingleLineComment

      if (currChar32 == U'\n') {
        state = State::End;
        comment_u8str.insert(0, "//");
        _emitToken(PPToken::createWhitespaceSequence(comment_u8str));
      } else {
        _toNext();
        comment_u8str += static_cast<char>(currChar32);
      }
    }

    else if (state == State::MultipleLineComment) {
      // *      =>  MultipleLineCommentStar
      // other  =>  MultipleLineComment
      _toNext();

      comment_u8str += static_cast<char>(currChar32);
      if (currChar32 == U'*')
        state = State::MultipleLineCommentStar;
    }

    else if (state == State::MultipleLineCommentStar) {
      // Previous: *
      // *      =>  MultipleLineCommentStar
      // /      =>  Prepend "/*' to comment_u8str, emit comment_u8str as a
      //            whitespace-sequence
      // other  =>  MultipleLineComment
      _toNext();

      comment_u8str += static_cast<char>(currChar32);
      if (currChar32 == U'*') {
        // no-op
      } else if (currChar32 == U'/') {
        state = State::End;
        comment_u8str.insert(0, "/*");
        _emitToken(PPToken::createWhitespaceSequence(comment_u8str));
      } else {
        state = State::MultipleLineComment;
      }
    }


    ////////////////////////////////////////////////////////////////////////////////
    // preprocessing-op-or-punc
    //
    // The preprocessing-op-or-puncs can be divided into four categories:
    // (Space separated, leading number in each line is the number of op-or-punc
    // in the line)
    //
    //     simple-op-or-punc: one of
    //       10  { } [ ] ( ) ? , ; /
    //
    //     equal-sign-op: one of
    //       2   * *=
    //       2   / /=
    //       2   ^ ^=
    //       2   ~ ~=
    //       2   ! !=
    //       2   = ==
    //
    //     stateful-op-or-punc: one of
    //       2   # ##
    //       3   | |= ||
    //       3   + += ++
    //       3   : :> ::
    //       3   & &= &&
    //       3   . .* ... (entry to pp-number)
    //       4   > >= >> >>=
    //       5   - -= -- -> ->*
    //       5   % %= %> %: %:%:
    //       6   < <= <% <: << <<=
    //
    //     identifier-like-op-or-punc: one of
    //       2   new delete
    //       11  and and_eq bitand bitor compl not not_eq or or_eq xor xor_eq
    //
    // The simple-op-or-puncs only have one char and can be emitted without
    // lookahead since no other op-or-punc has the same char as the first char.
    //
    // The equal-sign-op all have similar production, that is, an ASCII char, or
    // the same ASCII char followed by the equal sign. Programmatically, they
    // are be handled in a very compact manner.
    //
    // The stateful-op-or-puncs require extra DFA state(s) to parse. This is the
    // more complex part of parsing preprocessing-op-or-punc.
    //
    // The identifier-like-op-or-puncs are parsed in the identifier section of
    // the DFA but emitted as preprocessing-op-or-puncs.
    //
    // NOTE:
    //  When "#" or "%:" is emitted at the start of line, i.e.,
    //  _isBeginningOfLine is true, this is indeed a preprocessing directive. In
    //  this case, we need to set _isBeginningOfHeaderName to true. If an
    //  identifier "include" is emitted while _isBeginningOfHeaderName is true,
    //  we start parsing for header-name, i.e., go to State::HeaderName_Start.
    //
    ////////////////////////////////////////////////////////////////////////////////

    else if (state == State::EqualSignOp) {
      // Previous was a equal-sign-up, denote previous char as X
      // =      =>  Emit "X="
      // other  =>  Emit "X", curr PPCodeUnit is not consumed.
      if (currChar32 == U'=') {
        _toNext();
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc(equal_sign_op_u8str + "="));
      } else {
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc(equal_sign_op_u8str));
      }
    }

    else if (state == State::PoundSign) {
      // Previous: #
      // #      =>  Emit ##
      // other  =>  Emit #, set _isBeginningOfHeaderName to true if
      //            _isBeginningOfLine is true. The curr PPCodeUnit is not
      //            consumed.
      fprintf(stderr,"State::PoundSign <%02X>\n", currChar32);

      if (currChar32 == U'#') {
        _toNext();
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc("##"));
      } else {
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc("#"));
        if (_isBeginningOfLine)
          _isBeginningOfHeaderName = true;
      }
    }

    else if (state == State::VerticalBar) {
      // Previous: |
      // =      =>  Emit |=
      // |      =>  Emit ||
      // other  =>  Emit |, curr PPCodeUnit is not consumed.
      if (currChar32 == U'=') {
        _toNext();
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc("|="));
      } else if (currChar32 == U'|') {
        _toNext();
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc("||"));
      } else {
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc("|"));
      }
    }

    else if (state == State::Plus) {
      // Previous: +
      // =      =>  Emit +=
      // +      =>  Emit ++
      // other  =>  Emit +, curr PPCodeUnit is not consumed.
      if (currChar32 == U'=') {
        _toNext();
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc("+="));
      } else if (currChar32 == U'+') {
        _toNext();
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc("++"));
      } else {
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc("+"));
      }
    }

    else if (state == State::Minus) {
      // Previous: -
      // =      =>  Emit -=
      // -      =>  Emit --
      // >      =>  Minus2 (-> ->*)
      // other  =>  Emit -, curr PPCodeUnit is not consumed.
      if (currChar32 == U'=') {
        _toNext();
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc("-="));
      } else if (currChar32 == U'-') {
        _toNext();
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc("--"));
      } else if (currChar32 == U'>') {
        _toNext();
        state = State::Minus2;
      } else {
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc("-"));
      }
    }

    else if (state == State::Minus2) {
      // Previous: ->
      // *      =>  Emit ->*
      // other  =>  Emit ->, curr PPCodeUnit is not consumed.
      if (currChar32 == U'*') {
        _toNext();
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc("->*"));
      } else {
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc("->"));
      }
    }

    else if (state == State::Ampersand) {
      // Previous: &
      // =      =>  Emit &=
      // &      =>  Emit &&
      // other  =>  Emit &, curr PPCodeUnit is not consumed.
      if (currChar32 == U'=') {
        _toNext();
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc("&="));
      }  else if (currChar32 == U'&') {
        _toNext();
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc("&&"));
      } else {
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc("&"));
      }
    }

    else if (state == State::Bra) {
      // Previous:  <
      // =      =>  Emit <=
      // %      =>  Emit <%
      // :      =>  Emit <:
      // <      =>  BraBra
      // other  =>  Emit <, curr PPCodeUnit is not consumed.
      if (currChar32 == U'='  ||  currChar32 == U'%'  ||  currChar32 == U':') {
        state = State::End;
        _toNext();
        _emitToken(PPToken::createPreprocessingOpOrPunc(
              std::string("<") + std::string(1, static_cast<char>(currChar32))));
      } else {
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc("<"));
      }
    }

    else if (state == State::BraBra) {
      // Previous: <<
      // =      =>  Emit <<=
      // other  =>  Emit <<, curr PPCodeUnit is not consumed.
      if (currChar32 == U'=') {
        state = State::End;
        _toNext();
        _emitToken(PPToken::createPreprocessingOpOrPunc("<<="));
      } else {
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc("<<"));
      }
    }

    else if (state == State::Ket) {
      // Previous: >
      // =      => Emit >=
      // >      => KetKet
      // other  => Emit >, curr PPCodeUnit is not consumed.
      if (currChar32 == U'=') {
        _toNext();
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc(">="));
      } else if (currChar32 == U'>') {
        _toNext();
        state = State::KetKet;
      } else {
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc(">"));
      }
    }

    else if (state == State::KetKet) {
      // Previous: Ket >>
      // =      => Emit >>=
      // other  => Emit >>, curr PPCodeUnit is not consumed.
      if (currChar32 == U'=') {
        _toNext();
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc(">>="));
      } else {
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc(">>"));
      }
    }

    else if (state == State::Dot) {
      // Previous: .
      // .      =>  DotDot
      // *      =>  Emit .*
      // [0-9]  =>  PPNumber_LackDigit
      // other  =>  Emit ., curr PPCodeUnit is not consumed.

      if (currChar32 == U'*') {
        _toNext();
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc(".*"));
      } else if (currChar32 == U'.') {
        _toNext();
        state = State::DotDot;
      } else if (PPCodePointCheck::isDigit(currChar32)) {
        _toNext();
        ppnumber_u8str = ".";
        ppnumber_u8str += static_cast<char>(currChar32);
        state = State::PPNumber_LackDigit;
      } else {
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc("."));
      }
    }

    else if (state == State::DotDot) {
      // Previous state can only be State::Dot.
      //
      // Expect a dot . to emit ...
      //
      // Otherwise, report error because the C++ language does not permit
      // only two consecutive dots.
      _toNext();

      if (currChar32 == U'.') {
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc("..."));
      } else {
        state = State::Error;
        _setError(R"(Encountered two consecutive dots, expect a third dot)");
      }
    }

    else if (state == State::Column) {
      // Previous:  :
      // >      =>  Emit :>
      // other  =>  Emit :, curr PPCodeUnit is not consumed
      if (currChar32 == U'>') {
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc(":>"));
      } else if (PPCodePointCheck::isBasicSourceCharacter(currChar32)) {
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc(":"));
      } else {
        state = State::Error;
        _setError(R"(Not a basic-source-character)");
      }
    }

    else if (state == State::PercentSign) {
      // Previous:  %
      // >      =>  Emit %> as Digraph
      // :      =>  If nextChar32 is U'%', goto PercentSign2. Otherwise emit %:
      //            and set _isBeginningOfHeaderName if _isBeginningOfLine is
      //            true and the next PPCodeUnit is not consumed.
      // other  =>  Emit %, curr PPCodeUnit is not consumed.

      if (currChar32 == U'>') {
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc("%>"));
      } else if (currChar32 == U':') {
        const std::shared_ptr<PPCodeUnit> next = _stream->getCodeUnit();
        if (next->getChar32() == U'%') {
          _toNext();
          state = State::PercentSign2;
        } else {
          state = State::End;
          _emitToken(PPToken::createPreprocessingOpOrPunc("%:"));
          if (_isBeginningOfLine)
            _isBeginningOfHeaderName = true;
        }
      } else if (PPCodePointCheck::isBasicSourceCharacter(currChar32)) {
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc(":"));
      } else {
        state = State::Error;
        _setError(R"(Not a basic-source-character)");
      }
    }

    else if (state == State::PercentSign2) {
      // Previous:  %:%
      // :      =>  Emit %:%: as Digraph
      // other  =>  Emit "%:" as Digraph, and transition to PercentSign.
      //            The curr PPCodeUnit is not consumed.

      if (currChar32 == U':') {
        state = State::End;
        _emitToken(PPToken::createPreprocessingOpOrPunc("%:%:"));
      } else if (PPCodePointCheck::isBasicSourceCharacter(currChar32)) {
        state = State::PercentSign;
        _emitToken(PPToken::createPreprocessingOpOrPunc("%:"));
      } else {
        state = State::Error;
        _setError(R"(Not a basic-source-character)");
      }
    }


  } // while(1)

}
