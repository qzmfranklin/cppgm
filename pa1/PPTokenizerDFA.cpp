#include "PPCodePointCheck.h"
#include "PPCodeUnitCheck.h"
#include "PPTokenizerDFA.h"
#include <map>
#include <assert.h>

PPTokenizerDFA::PPTokenizerDFA(std::shared_ptr<PPCodeUnitStreamIfc> stream):
  _stream(stream), _isBeginningOfLine(true)
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

namespace {
  std::string _to_primary_form(const std::string &str)
  {
    static const std::map<std::string, std::string> map = {
      std::pair<std::string, std::string>("<%","{"),
      std::pair<std::string, std::string>("%>","}"),
      std::pair<std::string, std::string>("<:","["),
      std::pair<std::string, std::string>(":>","]"),
      std::pair<std::string, std::string>("%:","#"),
      std::pair<std::string, std::string>("%:%:","##"),

      std::pair<std::string, std::string>("and","&&"),
      std::pair<std::string, std::string>("bitor","|"),
      std::pair<std::string, std::string>("or","||"),
      std::pair<std::string, std::string>("xor","^"),
      std::pair<std::string, std::string>("compl","~"),
      std::pair<std::string, std::string>("bitand","&"),

      std::pair<std::string, std::string>("and_eq","&="),
      std::pair<std::string, std::string>("or_eq","|="),
      std::pair<std::string, std::string>("xor_eq","^="),
      std::pair<std::string, std::string>("not","!"),
      std::pair<std::string, std::string>("not_eq","!="),
    };
    const auto itr = map.find(str);
    return itr == map.end()  ?  str  : itr->second;
  }
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

    End,
    Error
  } state = State::Start;

  std::u32string u32str;
  std::string raw_string_u8str;
  std::string header_name_u8str;
  std::string ppnumber_u8str;

  fprintf(stderr,"here!\n");

  while (!_stream->isEmpty()  &&  state != State::End  &&  state != State::Error) {
    const std::shared_ptr<PPCodeUnit> curr = _stream->getCodeUnit();
    _stream->toNext();

    const PPCodeUnitType currType = curr->getType();

    const char32_t currChar32 = curr->getChar32();
    u32str += currChar32;

    if (state == State::Start) {
      fprintf(stderr,"Start: %s\n", curr->getUTF8String().c_str());

      if (currChar32 == U'\n') {
        // The begining of a line is either immediately after a newline
        // character \n or is at the start of the input stream.
        State = State::End;
        _queue.push(PPToken::createNewLine());
        _isBeginningOfLine = true;
      }

      else if (currType == PPCodeUnitType::WhitespaceCharacter) {
        // Whitespace characters are reverted in raw strings. Checks are needed
        // to verify that
        if (_isRawStringMode) {
          raw_string_u8str += curr->getUTF8String();
        }
        state = Start::End;
      }

      else if (_isBeginningOfLine) {
        // If the first non-whitespace- character, i.e., a PPCodeUnit that is
        // not PPCodeUnitWhitespaceCharacter, is the # or the digraph :%, this
        // line is a preprocessing directive and _isPreprocessingDirective is
        // set to true.
        //
        // In the Identifier section, if an identifier is issued while
        // _isPreprocessingDirective is true, and the identifier is 'include',
        // in addition to emitting the identifier 'include', the DFA transitions
        // to State::HeaderName to start parsing for header-names.
        _isBeginningOfLine = false;
        if (_to_primary_form(curr->getUTF8String()) == "#") {
          state = State::End;
          _queue.push(PPTokenType::createPreprocessingOpOrPunc("#"));
          _isPreprocessingDirective = true;
        }
      }

      else if (currChar32 == U'.') {
        // A dot . can start any of the four preprocessing tokens:
        //    .
        //    .*
        //    ...
        //    PPNumber_LackDigit, expects a digit to form a valid PPNumber
        //
        // Only a single lookahead PPCodeUnit is needed to determine which one
        // exactly this dot starts.

        std::shared_ptr<PPCodeUnit> p = _stream->getCodeUnit();

        if (p->getChar32() == U'*') {
          // .*, emit PPTokenPreprocessingOpOrPunc(".*")
          state = State::End;
          _queue.push(PPToken::createPreprocessingOpOrPunc(".*"));
          _stream->toNext();
        } else if (p->getChar32() == U'.') {
          // .., expect a following dot to form ...  Check the next PPCodeUnit,
          // if it is the third dot, emit PPTokenPreprocessingOpOrPunc("..."),
          // otherwise report error.
          _stream->toNext();
          p = _stream->getCodeUnit();
          if (p->getChar32() == U'.') {
            state = State::End;
            _queue.push(PPToken::createPreprocessingOpOrPunc("..."));
            _stream->toNext();
          } else {
            state = State::End;
            _queue.push(PPToken::createPreprocessingOpOrPunc("."));
            _queue.push(PPToken::createPreprocessingOpOrPunc("."));
          }
        } else if (PPCodeUnitCheck::isDigit(p)) {
          // Dot followed by digit, transition into PPNumber_LackDigit state.
          state = State::PPNumber_LackDigit;
        } else {
          // Emit PPTokenPreprocessingOpOrPunc(".");
          state = State::End;
          _queue.push(PPToken::createPreprocessingOpOrPunc("."));
        }

      }


      //else if (currChar32 == U'') {
      //}

    } // State::Start

    ////////////////////////////////////////////////////////////////////////////////
    // header-name
    //
    // Entry:
    //   The #include preprocessing directive ensuing a fresh new-line
    ////////////////////////////////////////////////////////////////////////////////
    // header-name start
    else if (state == State::HeaderName_Start) {
      // Expect < or "


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
    //else if (state == State::Identifier) {
      // Expect one of the following:
      //   digit, nondigit, and universal-character-names that are in AnnexE1
    //}

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
    //     stateless-op-or-punc: one of
    //       3   | |= ||
    //       3   + += ++
    //       3   : :> ::
    //       3   & &= &&
    //       3   . .* ... (entry to pp-number)
    //
    //       2   * *=
    //       2   / /=
    //       2   ^ ^=
    //       2   ~ ~=
    //       2   ! !=
    //       2   = ==
    //       2   # ##
    //
    //       2   > >=
    //       3   - -= --
    //       3   % %= %>
    //       3   < <= <% <:
    //
    //     stateful-op-or-punc: one of
    //       2   >> >>=
    //       2   -> ->*
    //       2   %: %:%:
    //       2   << <<=
    //
    //     identifier-like-op-or-punc: one of
    //       2   new delete
    //       11  and and_eq bitand bitor compl not not_eq or or_eq xor xor_eq
    //
    // The simple-op-or-puncs only have one char and can be emitted without
    // lookahead since no other op-or-punc has the same char as the first char.
    //
    // The stateless-op-or-puncs only need a one-curr lookahead to determine
    // the next state.
    //
    // The stateful-op-or-puncs require extra DFA state(s) to parse.
    //
    // The identifier-like-op-or-puncs are parsed in the identifier section of
    // the DFA but emitted as preprocessing-op-or-puncs.
    //
    ////////////////////////////////////////////////////////////////////////////////

  } // while(1)

}
