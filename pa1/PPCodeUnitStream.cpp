#include "PPCodePointCheck.h"
#include "PPCodeUnitStream.h"
#include <assert.h>


PPCodeUnitStream::PPCodeUnitStream(std::shared_ptr<UTF32StreamIfc> u32stream):
  _u32stream(u32stream)
{
  _pushCodeUnits();
}

bool PPCodeUnitStream::isEmpty() const
{
  return _u32stream->isEmpty()  &&  _queue.empty();
}

const std::shared_ptr<PPCodeUnit> PPCodeUnitStream::getCodeUnit() const
{
  assert(!_queue.empty());
  return _queue.front();
}

std::string PPCodeUnitStream::getErrorMessage() const
{
  return _errorMessage;
}

void PPCodeUnitStream::_setError(const std::string &&u8str)
{
  _errorMessage = std::move(u8str);
}

void PPCodeUnitStream::_clearError()
{
  _errorMessage.clear();
}

void PPCodeUnitStream::toNext()
{
  assert(!_queue.empty());
  _queue.pop();
  if (_queue.empty())
    _pushCodeUnits();
}

void PPCodeUnitStream::_pushCodeUnits()
{
  enum class State {
    Start,

    SingleQuad,
    DoubleQuad,
    SingleLineComment,
    MultipleLineCommentStart,
    MultipleLineComment,
    MultipleLineCommentStar,

    LeftParenthesis,
    Column,
    PercentSign,
    PercentSign2,
    PercentSign3,

    End,
    Error
  };

  std::u32string u32str;
  State state = State::Start;
  _clearError();
  while(state != State::End  &&  state != State::Error) {
    if (_u32stream->isEmpty())
      break;

    const char32_t curr32 = _u32stream->getChar32();
    _u32stream->toNext();

    if (state == State::Start) {
      if (curr32 == U'\\') { // Line splicing, universal-character-name
        const char32_t next32 = _u32stream->getChar32();
        if (next32 == U'\n') { // Line splicing
          // Pass
        } else if (next32 == U'u') { // \uXXXX
          state = State::SingleQuad;
        } else if (next32 == U'U') { // \UXXXXXXXX
          state = State::DoubleQuad;
        } else {
          state = State::Error;
          _setError(R"(Illegal character following \)");
        }
        _u32stream->toNext();
      } else if (curr32 == U'/') { // Comments
        const char32_t next32 = _u32stream->getChar32();
        if (next32 == U'/') { // Single-line comment
          state = State::SingleLineComment;
        } else if (next32 == U'*') { // Multiple-line comment
          state = State::MultipleLineCommentStart;
        } else {
          state = State::Error;
          _setError(R"(Illegal character following /)");
        }
        _u32stream->toNext();
      } else if (curr32 == U'<') { // Possibly <%, <:
        state = State::LeftParenthesis;
      } else if (curr32 == U':') { // Possibly :>
        state = State::Column;
      } else if (curr32 == U'%') { // Possibly %> %: %:%:
        state = State::PercentSign;
      } else if (PPCodePointCheck::isBasicSourceCharacter(curr32)) {
        _queue.push(PPCodeUnit::createASCIIChar(static_cast<const char>(curr32)));
        state = State::End;
      } else {
        _setError(R"(Not a basic-source-character)");
        state = State::Error;
      }
    }

    else if (state == State::SingleQuad) {
      std::string u8str(1, static_cast<char>(curr32));
      for (int i = 0; i < 3; i++) {
        const char32_t ch32 = _u32stream->getChar32();
        _u32stream->toNext();
        if (PPCodePointCheck::isHexadecimalDigit(ch32)) {
          u8str  += static_cast<char>(ch32);
        } else {
          state = State::Error;
          _setError(R"(Illegal character following \u: )" + std::string(1, static_cast<char>(ch32)));
        }
      }
      const char32_t value = static_cast<char32_t>(std::stoull(u8str, nullptr, 16));
      _queue.push(PPCodeUnit::createUniversalCharacterName(value, std::string("\\u") + u8str));
      state = State::End;
    }

    else if (state == State::DoubleQuad) {
      std::string u8str(1, static_cast<char>(curr32));
      for (int i = 0; i < 7; i++) {
        const char32_t ch32 = _u32stream->getChar32();
        _u32stream->toNext();
        if (PPCodePointCheck::isHexadecimalDigit(ch32)) {
          u8str  += static_cast<char>(ch32);
        } else {
          state = State::Error;
          _setError(R"(Illegal character following \U)" + std::string(1, static_cast<char>(ch32)));
        }
      }
      const char32_t value = static_cast<char32_t>(std::stoull(u8str, nullptr, 16));
      _queue.push(PPCodeUnit::createUniversalCharacterName(value, std::string("\\U") + u8str));

      state = State::End;
    }

    else if (state == State::SingleLineComment) {
      u32str = U"//";
      u32str += curr32;
      while(1) {
        if (_u32stream->isEmpty()) {
          state = State::Error;
          _setError(R"(Reached end-of-file in single-line-comment)");
          break;
        }

        const char32_t ch32 = _u32stream->getChar32();
        if (ch32 == U'\n') {
          _queue.push(PPCodeUnit::createComment(u32str));
          state = State::End;
          break;
        } else {
          u32str += ch32;
          _u32stream->toNext();
        }
      }
    }

    else if (state == State::MultipleLineCommentStart) {
      // *      =>  MultipleLineComment
      // other  =>  Error
      u32str = U"/*";
      u32str += curr32;
      state = State::MultipleLineComment;
    }

    else if (state == State::MultipleLineComment) {
      // *      =>  MultipleLineCommentStar
      // other  =>  MultipleLineComment
      u32str += curr32;
      if (curr32 == U'*')
        state = State::MultipleLineCommentStar;
    }

    else if (state == State::MultipleLineCommentStar) {
      // *      =>  MultipleLineCommentStar
      // /      =>  End
      // other  =>  MultipleLineComment
      u32str += curr32;

      if (curr32 == U'*') {
        //state = State::MultipleLineCommentStar;
      } else if (curr32 == U'/') {
        state = State::End;
        _queue.push(PPCodeUnit::createComment(u32str));
      } else {
        state = State::MultipleLineComment;
      }
    }

    else if (state == State::LeftParenthesis) {
      // Previous:  <
      // %      =>  Emit <% as Digraph
      // :      =>  Emit <: as Digraph
      // other  =>  Emit U'<' and curr32 as ASCIIChar
    }

    else if (state == State::Column) {
      // Previous:  :
      // >      =>  Emit :> as Digraph
      // other  =>  Emit U':' and curr32 as ASCIIChar
    }

    else if (state == State::PercentSign) {
      // Previous:  %
      // >      =>  Emit %> as Digraph
      // :      =>  Emit U'%' and U':' as ASCIIChar, or
      //            PercentSign2 otherwise, if next32 is U'%'
      // other  =>  Emit U'%' and curr32 as ASCIIChar
    }

    else if (state == State::PercentSign2) {
      // Previous:  %:%
      // :      =>  Emit %:%: as Digraph
      // other  =>  Emit U'%', U':', U'%', and curr32 as ASCIIChar
      //
      // For example: %:%d is converted to {'%', ':', '%', 'd'}, not {'%:', '%',
      // 'd'}.
    }

    else if (state == State::PercentSign3) {
      // Previous:  %:%
      // :      =>  Emit %:%: as Digraph
      // other  =>  Emit U'%', U':', and curr32 as ASCIIChar
    }

    else if (state == State::End) {
      state = State::Error;
      _setError(R"(Reached an impossible state State::End)");
    }

    else if (state == State::Error) {
      state = State::Error;
      _setError(R"(Reached an impossible state State::Error)");
    }

  } // while
}
