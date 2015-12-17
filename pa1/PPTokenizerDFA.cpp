#include "PPCodePointCheck.h"
#include "PPCodeUnitCheck.h"
#include "PPTokenizerDFA.h"
#include <map>
#include <assert.h>

PPTokenizerDFA::PPTokenizerDFA(std::shared_ptr<PPCodeUnitStreamIfc> stream):
  _stream(stream)
{
  fprintf(stderr,"Ctor0\n");
  _pushTokens();
  fprintf(stderr,"Ctor1\n");
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
    Start,

    HeaderName_Start,
    HeaderName_HSequenceStart,
    HeaderName_HSequence,
    HeaderName_QSequenceStart,
    HeaderName_QSequence,

    PPNumber_LackDigit,
    PPNumber_Exponent,
    PPNumber_Apostrophe,
    PPNumber,

    Identifier,

    SingleLineComment,
    MultipleLineComment_Start,
    MultipleLineComment,

    EqualSignOp,

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

  while (!_stream->isEmpty()  &&  state != State::End  &&  state != State::Error) {
    // In a state block, e.g., the block for if (state == State::SomeState), the
    // developer needs to call _stream->toNext() explicitly otherwise the stream
    // PPCodeUnitStream object will NOT move forward.
    //
    // The reason for doing this is to allow a state to not consume the symbol
    // being processed, e.g., in State::LeftParenthesis.
    const std::shared_ptr<PPCodeUnit> curr = _stream->getCodeUnit();
    const char32_t currChar32 = curr->getChar32();

    if (state == State::Start) {
      // State::Start means starting to parse and emit the next PPToken.
      // Specifically, this does not imply start of line/file, although in
      // certain cases it could be a start of line/file.
      _stream->toNext();

      fprintf(stderr,"Start: %s\n", curr->getUTF8String().c_str());

      if (currChar32 == U'\n') {
        // The begining of a line is either immediately after a newline
        // character \n or is at the start of the input stream.
        state = State::End;
        _queue.push(PPToken::createNewLine());
        _isBeginningOfLine = true;
        _isPreprocessingDirective = false;
      }

      else if (currChar32 == U'/') { // Comments
        if (_stream->isEmpty()) {
          state = State::Error;
          _setError(R"(Expecting / or * after a backslash /)");
          continue;
        }

        const std::shared_ptr<PPCodeUnit> next = _stream->getCodeUnit();
        _stream->toNext();
        const std::string str = next->getUTF8String();
        if (str == "/") { // Single-line comment
          state = State::SingleLineComment;
        } else if (str == "*") { // Multiple-line comment
          state = State::MultipleLineComment_Start;
        } else {
          state = State::Error;
          _setError(R"(Illegal sequence following /)");
        }
      } // else if (currChar32 == U'/')

      // simple-op-or-punc
      else if (currChar32 == U'{'  ||  currChar32 == U'}'  ||  currChar32 == U'['
          ||   currChar32 == U']'  ||  currChar32 == U'('  ||  currChar32 == U')'
          ||   currChar32 == U'?'  ||  currChar32 == U','  ||  currChar32 == U';'
          ||   currChar32 == U'/') {
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc(std::string(1, static_cast<char>(currChar32))));
      }

      // equal-sign-punc
      else if (currChar32 == U'*'  ||  currChar32 == U'/'  ||  currChar32 == U'^'
          ||   currChar32 == U'~'  ||  currChar32 == U'!'  ||  currChar32 == U'=') {
        state = State::EqualSignOp;
        equal_sign_op_u8str = std::string(1, static_cast<char>(currChar32));
      }

      // stateful-op-or-punc
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
      } else if (currChar32 == U'<') { // < <= <% <: << <<=
        state = State::Bra;
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
        identifier_u8str = curr->getUTF8String();
        state = State::Identifier;
      }

      //else if (!PPCodePointCheck::isBasicSourceCharacter(currChar32)) {
        //state = State::End;
        //_queue.push(PPToken::createNonWhitespaceChar(std::u32string(currChar32)));
      //}


      //else if (currChar32 == U'') {
      //}

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
      // other                =>  Emit identifier
      //
      // When emitting an identifier, if _isBeginningOfLine is true and the
      // identifier is "include", set _isPreprocessingDirective to true.
      _stream->toNext();

      identifier_u8str += curr->getUTF8String();
      if (!PPCodeUnitCheck::isIdentifierNonStart(curr)) {
        state = State::End;
        _queue.push(PPToken::createIdentifier(identifier_u8str));
        if (_isBeginningOfLine  &&  identifier_u8str == "include")
          _isPreprocessingDirective = true;
      }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // header-name
    //
    // Entry:
    //   The #include preprocessing directive ensuing a fresh new-line
    ////////////////////////////////////////////////////////////////////////////////
    // header-name start
    else if (state == State::HeaderName_Start) {
      // Expect < or "
      _stream->toNext();


      if (currChar32 == U'<') {
        // <bracket_header>
        header_name_u8str = "<";
        state = State::HeaderName_HSequenceStart;
      } else if (currChar32 == U'\"') {
        // "quote_header"
        header_name_u8str = "\"";
        state = State::HeaderName_QSequenceStart;
      } else {
        state = State::Error;
        _setError(
            R"(Expecting either < or " to start parsing the header name sequence.)");
      }
    }

    // header-name h-char-sequence
    if (state == State::HeaderName_HSequenceStart) {
      // Expect an h-char
      _stream->toNext();

      if (PPCodePointCheck::isNotHChar(currChar32)) {
        state = State::Error;
        _setError(
            R"(Expecting an initial h-char for the header name.)");
      } else {
        state = State::HeaderName_HSequence;
        header_name_u8str += static_cast<char>(currChar32);
      }
    }

    if (state == State::HeaderName_HSequence) {
      // Expect an h-char or >
      _stream->toNext();

      if (currChar32 == U'>') {
        state = State::End;
        header_name_u8str += '>';
        _queue.push(PPToken::createHeaderName(header_name_u8str));
      } else if (PPCodePointCheck::isNotHChar(currChar32)) {
        state = State::Error;
        _setError(
            R"(Expecting an initial h-char to continue the header name or > to terminate the header name.)");
      } else {
        header_name_u8str += static_cast<char>(currChar32);
      }
    }

    // header-name q-char-sequence
    if (state == State::HeaderName_QSequenceStart) {
      // Expect an q-char
      _stream->toNext();

      if (PPCodePointCheck::isNotHChar(currChar32)) {
        state = State::Error;
        _setError(
            R"(Expecting an initial q-char for the header name.)");
      } else {
        state = State::HeaderName_QSequence;
        header_name_u8str += static_cast<char>(currChar32);
      }
    }

    if (state == State::HeaderName_QSequence) {
      // Expect an q-char or "
      _stream->toNext();

      if (currChar32 == U'\"') {
        state = State::End;
        header_name_u8str += '\"';
        _queue.push(PPToken::createHeaderName(header_name_u8str));
      } else if (PPCodePointCheck::isNotHChar(currChar32)) {
        state = State::Error;
        _setError(
            R"(Expecting an initial q-char to continue the header name or > to terminate the header name.)");
      } else {
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
    else if (state == State::PPNumber_LackDigit) {
      // Expect a digit. Previous curr was a dot, transition to PPNumber
      _stream->toNext();

      if (PPCodePointCheck::isDigit(currChar32)) {
        state = State::PPNumber;
        ppnumber_u8str += static_cast<char>(currChar32);
      } else {
        state = State::Error;
        _setError(R"(Expecting a digit to form a pp-number)");
      }
    }

    else if (state == State::PPNumber_Exponent) {
      // Expect a sign. Previous curr was e or E, transition to PPNumber
      _stream->toNext();

      if (PPCodePointCheck::isSign(currChar32)) {
        state = State::PPNumber;
        ppnumber_u8str += static_cast<char>(currChar32);
      } else {
        _setError(
            R"(Expect a sign, i.e., either + or -, to transition to pp-number.)");
      }
    }

    else if (state == State::PPNumber_Apostrophe) {
      // Expect a digit or a nondigit. Previous curr was ', transition to PPNumber
      _stream->toNext();

      if (PPCodePointCheck::isDigit(currChar32) || PPCodePointCheck::isNondigit(currChar32)) {
        state = State::PPNumber;
        ppnumber_u8str += static_cast<char>(currChar32);
      } else if (PPCodePointCheck::isNotHChar(currChar32)) {
        state = State::Error;
        _setError(
            R"(Expect a digit or a nondigit to transition to pp-number.)");
      } else {
        header_name_u8str += '\"';
        _queue.push(PPToken::createHeaderName(header_name_u8str));
       }
    }

    else if (state == State::PPNumber) {
      // Expect one of the following:
      //   ., digit, identifier-nondigit   =>  PPNumber
      //   '                               =>  PPNumber_Apostrophe
      //   e, E                            =>  PPNumber_Exponent
      _stream->toNext();

      if (currChar32 == U'e'  ||  currChar32 == U'E') {
        state = State::PPNumber_Exponent;
        ppnumber_u8str += static_cast<char>(currChar32);
      } else if (currChar32 == U'\'') {
        state = State::PPNumber_Apostrophe;
        ppnumber_u8str += static_cast<char>(currChar32);
      } else if (currChar32 == U'.'  ||  PPCodePointCheck::isDigit(currChar32)  ||  PPCodeUnitCheck::isIdentifierNondigit(curr)) {
        ppnumber_u8str += static_cast<char>(currChar32);
      } else {
        state = State::Error;
        _setError(
            R"(Expecting one of the following: ., digit, identifier-nondigit, ', e, E)");
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

    else if (state == State::SingleLineComment) {
      // Consumes possibly more than one character until the first newline
      // character. The newline character is not part of the comment and is not
      // consumed.
      //
      // Note that the parsing of the newline character that terminated this
      // single line comment is delayed to the next call of te _pushTokens()
      // method.
      _stream->toNext();

      comment_u32str = U"//";
      comment_u32str += currChar32;
      while(1) {
        if (_stream->isEmpty()) {
          // The UTF32StreamIfc guarantees that the last character is a new-line
          // character. Therefore, it is impossible to reach the end of file before
          // a newline character along normal paths.
          state = State::Error;
          _setError(R"(Reached end-of-file in single-line-comment)");
          break;
        }

        const std::shared_ptr<PPCodeUnit> next = _stream->getCodeUnit();
        if (next->getChar32() == U'\n') {
          state = State::End;
          break;
        } else {
          comment_u32str += next->getChar32();
          _stream->toNext();
        }
      }
    }

    else if (state == State::MultipleLineComment_Start) {
      // *      =>  MultipleLineComment
      // other  =>  Error
      _stream->toNext();

      comment_u32str = U"/*";
      comment_u32str += currChar32;
      state = State::MultipleLineComment;
    }

    else if (state == State::MultipleLineComment) {
      // *      =>  MultipleLineComment_Start
      // other  =>  MultipleLineComment
      _stream->toNext();

      comment_u32str += currChar32;
      if (currChar32 == U'*')
        state = State::MultipleLineComment_Start;
    }

    else if (state == State::MultipleLineComment_Start) {
      // *      =>  MultipleLineComment_Start
      // /      =>  End
      // other  =>  MultipleLineComment
      _stream->toNext();

      comment_u32str += currChar32;
      if (currChar32 == U'*') {
        //state = State::MultipleLineComment_Start;
      } else if (currChar32 == U'/') {
        state = State::End;
        _queue.push(PPToken::createWhitespaceSequence(comment_u32str));
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
    //  this case, we need to set _isPreprocessingDirective to true. If an
    //  identifier "include" is emitted while _isPreprocessingDirective is true,
    //  we start parsing for header-name, i.e., go to State::HeaderName_Start.
    //
    ////////////////////////////////////////////////////////////////////////////////

    else if (state == State::EqualSignOp) {
      // Previous was a equal-sign-up, denote previous char as X
      // =      =>  Emit "X="
      // other  =>  Emit "X", curr PPCodeUnit is not consumed.
      if (currChar32 == U'=') {
        _stream->toNext();
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc(equal_sign_op_u8str + "="));
      } else {
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc(equal_sign_op_u8str));
      }
    }

    else if (state == State::PoundSign) {
      // Previous: #
      // #      =>  Emit ##
      // other  =>  Emit #, set _isPreprocessingDirective to true if
      //            _isBeginningOfLine is true. The curr PPCodeUnit is not
      //            consumed.
      if (currChar32 == U'#') {
        _stream->toNext();
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc("##"));
      } else {
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc("#"));
        if (_isBeginningOfLine)
          _isPreprocessingDirective = true;
      }
    }

    else if (state == State::VerticalBar) {
      // Previous: |
      // =      =>  Emit |=
      // |      =>  Emit ||
      // other  =>  Emit |, curr PPCodeUnit is not consumed.
      if (currChar32 == U'=') {
        _stream->toNext();
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc("|="));
      } else if (currChar32 == U'|') {
        _stream->toNext();
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc("||"));
      } else {
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc("|"));
      }
    }

    else if (state == State::Plus) {
      // Previous: +
      // =      =>  Emit +=
      // +      =>  Emit ++
      // other  =>  Emit +, curr PPCodeUnit is not consumed.
      if (currChar32 == U'=') {
        _stream->toNext();
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc("+="));
      } else if (currChar32 == U'+') {
        _stream->toNext();
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc("++"));
      } else {
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc("+"));
      }
    }

    else if (state == State::Minus) {
      // Previous: -
      // =      =>  Emit -=
      // -      =>  Emit --
      // >      =>  Minus2 (-> ->*)
      // other  =>  Emit -, curr PPCodeUnit is not consumed.
      if (currChar32 == U'=') {
        _stream->toNext();
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc("-="));
      } else if (currChar32 == U'-') {
        _stream->toNext();
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc("--"));
      } else if (currChar32 == U'>') {
        _stream->toNext();
        state = State::Minus2;
      } else {
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc("-"));
      }
    }

    else if (state == State::Minus2) {
      // Previous: ->
      // *      =>  Emit ->*
      // other  =>  Emit ->, curr PPCodeUnit is not consumed.
      if (currChar32 == U'*') {
        _stream->toNext();
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc("->*"));
      } else {
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc("->"));
      }
    }

    else if (state == State::Ampersand) {
      // Previous: &
      // =      =>  Emit &=
      // &      =>  Emit &&
      // other  =>  Emit &, curr PPCodeUnit is not consumed.
      if (currChar32 == U'=') {
        _stream->toNext();
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc("&="));
      }  else if (currChar32 == U'&') {
        _stream->toNext();
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc("&&"));
      } else {
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc("&"));
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
        _stream->toNext();
        _queue.push(PPToken::createPreprocessingOpOrPunc(
              std::string("<") + std::string(1, static_cast<char>(currChar32))));
      } else {
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc("<"));
      }
    }

    else if (state == State::BraBra) {
      // Previous: <<
      // =      =>  Emit <<=
      // other  =>  Emit <<, curr PPCodeUnit is not consumed.
      if (currChar32 == U'=') {
        state = State::End;
        _stream->toNext();
        _queue.push(PPToken::createPreprocessingOpOrPunc("<<="));
      } else {
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc("<<"));
      }
    }

    else if (state == State::Dot) {
      // Previous: .
      // .      =>  DotDot
      // *      =>  Emit .*
      // [0-9]  =>  PPNumber_LackDigit
      // other  =>  Emit ., curr PPCodeUnit is not consumed.

      if (currChar32 == U'*') {
        _stream->toNext();
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc(".*"));
      } else if (currChar32 == U'.') {
        _stream->toNext();
        state = State::DotDot;
      } else if (PPCodeUnitCheck::isDigit(curr)) {
        _stream->toNext();
        state = State::PPNumber_LackDigit;
      } else {
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc("."));
      }
    }

    else if (state == State::DotDot) {
      // Previous state can only be State::Dot.
      //
      // Expect a dot . to emit ...
      //
      // Otherwise, report error because the C++ language does not permit
      // only two consecutive dots.
      _stream->toNext();

      if (currChar32 == U'.') {
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc("..."));
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
        _queue.push(PPToken::createPreprocessingOpOrPunc(":>"));
      } else if (PPCodePointCheck::isBasicSourceCharacter(currChar32)) {
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc(":"));
      } else {
        state = State::Error;
        _setError(R"(Not a basic-source-character)");
      }
    }

    else if (state == State::PercentSign) {
      // Previous:  %
      // >      =>  Emit %> as Digraph
      // :      =>  If nextChar32 is U'%', goto PercentSign2. Otherwise emit %:
      //            and set _isPreprocessingDirective if _isBeginningOfLine is
      //            true and the next PPCodeUnit is not consumed.
      // other  =>  Emit %, curr PPCodeUnit is not consumed.

      if (currChar32 == U'>') {
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc("%>"));
      } else if (currChar32 == U':') {
        const std::shared_ptr<PPCodeUnit> next = _stream->getCodeUnit();
        if (next->getChar32() == U'%') {
          _stream->toNext();
          state = State::PercentSign2;
        } else {
          state = State::End;
          _queue.push(PPToken::createPreprocessingOpOrPunc("%:"));
          if (_isBeginningOfLine)
            _isPreprocessingDirective = true;
        }
      } else if (PPCodePointCheck::isBasicSourceCharacter(currChar32)) {
        state = State::End;
        _queue.push(PPToken::createPreprocessingOpOrPunc(":"));
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
        _queue.push(PPToken::createPreprocessingOpOrPunc("%:%:"));
      } else if (PPCodePointCheck::isBasicSourceCharacter(currChar32)) {
        state = State::PercentSign;
        _queue.push(PPToken::createPreprocessingOpOrPunc("%:"));
      } else {
        state = State::Error;
        _setError(R"(Not a basic-source-character)");
      }
    }


  } // while(1)

}
