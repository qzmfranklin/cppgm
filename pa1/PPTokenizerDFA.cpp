#include "PPCodePointCheck.h"
#include "PPCodeUnitCheck.h"
#include "PPTokenizerDFA.h"
#include <map>
#include <assert.h>

// Comment the following line to see all sorts of debug information
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

    NumberOfStates, // Convenience for statistics

    End,
    Error
  } state = State::Start;

  // String variables used by the DFA.
  std::u32string comment_u32str;
  std::string raw_string_u8str;
  std::string header_name_u8str;
  std::string ppnumber_u8str;
  std::string equal_sign_op_u8str;
  std::string identifier_u8str;

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

      // raw strings
      // header-name q-sequence, e.g., "my_lib.h" enters here too
      else if (currChar32 == U'\"') {
        if (_isBeginningOfHeaderName) {
          state = State::HeaderNameQ;
        } else {
          // handle raw strings here...
        }
      }

      else if (PPCodePointCheck::isDigit(currChar32)) {
        ppnumber_u8str = static_cast<char>(currChar32);
        state = State::PPNumber;
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
      // other                =>  Emit identifier, the curr PPCodeUnit is not
      //                          consumed.
      //
      // When emitting an identifier, if _isBeginningOfLine is true and the
      // identifier is "include", set _isBeginningOfHeaderName to true.
      fprintf(stderr,"State::Identifier\n");

      if (PPCodeUnitCheck::isIdentifierNonStart(curr)) {
        _toNext();
        identifier_u8str += curr->getUTF8String();
      } else {
        state = State::End;
        _emitToken(PPToken::createIdentifier(identifier_u8str));
        if (identifier_u8str == "include") {
          if (_isBeginningOfLine) {
            _isBeginningOfHeaderName = true;
          } else {
            state = State::Error;
            _setError(R"(The identifier "include" can only be used following # at the beginning of a new line)");
          }
        }
      }
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
      // \n     =>  Prepend "//' to comment_u32str, emit comment_u32str as
      //            whitespace-sequence
      // other  =>  SingleLineComment

      if (currChar32 == U'\n') {
        state = State::End;
        comment_u32str.insert(0, U"//");
        _emitToken(PPToken::createWhitespaceSequence(comment_u32str));
      } else {
        _toNext();
        comment_u32str += currChar32;
      }
    }

    else if (state == State::MultipleLineComment) {
      // *      =>  MultipleLineCommentStar
      // other  =>  MultipleLineComment
      _toNext();

      comment_u32str += currChar32;
      if (currChar32 == U'*')
        state = State::MultipleLineCommentStar;
    }

    else if (state == State::MultipleLineCommentStar) {
      // Previous: *
      // *      =>  MultipleLineCommentStar
      // /      =>  Prepend "/*' to comment_u32str, emit comment_u32str as a
      //            whitespace-sequence
      // other  =>  MultipleLineComment
      _toNext();

      comment_u32str += currChar32;
      if (currChar32 == U'*') {
        // no-op
      } else if (currChar32 == U'/') {
        state = State::End;
        comment_u32str.insert(0, U"/*");
        _emitToken(PPToken::createWhitespaceSequence(comment_u32str));
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
