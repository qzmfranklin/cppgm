#include "PPCodePointCheck.h"
#include "PPCodeUnitStream.h"
#include <assert.h>

// Comment out this following line to see debug prints.
#define fprintf(stderr,...)

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
  assert(_queue.empty());

  enum class State {
    Start,

    Backslash,

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

    End,
    Error
  };

  std::string single_quad_u8str;
  std::string double_quad_u8str;

  const auto _toNext = [this] () {
    //const char32_t curr = this->_u32stream->getChar32();
    this->_u32stream->toNext();
    //const char32_t next = this->_u32stream->getChar32();
    //fprintf(stderr,"U+%06X <%c> => U+%06X <%c>\n",
        //static_cast<uint32_t>(curr), static_cast<char>(curr),
        //static_cast<uint32_t>(next), static_cast<char>(next));
  };

  const auto _emitCodeUnit = [this] (const std::shared_ptr<PPCodeUnit> ptr) {
    fprintf(stderr,"_emitCodeUnit <%s>\n", ptr->getUTF8String().c_str());
    this->_queue.push(ptr);
  };

  std::u32string u32str;
  State state = State::Start;
  _clearError();
  while(!_u32stream->isEmpty()  &&  state != State::End  &&  state != State::Error) {
    const char32_t curr32 = _u32stream->getChar32();
    fprintf(stderr,"\n==PPCodeUnitStream== U+%06X <%c>\n",
        static_cast<uint32_t>(curr32), static_cast<char>(curr32));

    if (state == State::Start) {
      _toNext();
      fprintf(stderr,"State::Start\n");
      if (curr32 == U'\\') { // Line splicing, universal-character-name
        state = State::Backslash;
      } else if (PPCodePointCheck::isBasicSourceCharacter(curr32)) {
        _emitCodeUnit(PPCodeUnit::createASCIIChar(static_cast<const char>(curr32)));
        state = State::End;
      } else {
        state = State::End;
        _emitCodeUnit(PPCodeUnit::createNonASCIIChar(curr32));
      }
    }

    else if (state == State::Backslash) {
      fprintf(stderr,"State::Backslash\n");
      if (curr32 == U'\n') { // Line splicing state = State::End;
        _toNext();
        _emitCodeUnit(PPCodeUnit::createUniversalCharacterName(0, R"(\n)"));
      } else if (curr32 == U'u') { // \uXXXX
        _toNext();
        state = State::SingleQuad;
        single_quad_u8str.clear();
      } else if (curr32 == U'U') { // \UXXXXXXXX
        _toNext();
        state = State::DoubleQuad;
        double_quad_u8str.clear();
      } else if (PPCodePointCheck::isBasicSourceCharacter(curr32)) {
        state = State::End;
        _emitCodeUnit(PPCodeUnit::createASCIIChar('\\'));
      } else {
        state = State::Error;
        _setError(R"(Illegal character following \)");
      }
    }

    else if (state == State::SingleQuad) {
      fprintf(stderr,"State::SingleQuad\n");

      if (single_quad_u8str.length() == 4) {
        // Emit the universal-character-name the hex-quad is filled.
        state = State::End;
        const char32_t value = static_cast<char32_t>(std::stoull(single_quad_u8str, nullptr, 16));
        _emitCodeUnit(PPCodeUnit::createUniversalCharacterName(value, std::string("\\u") + single_quad_u8str));
      } else if (single_quad_u8str.length() > 4) {
        // Impossible to reach this state given the structure of this DFA.
        state = State::Error;
        _setError(R"(PPCodeUnitStream reached a seemingly impossible internal state.)");
      } else if (PPCodePointCheck::isHexadecimalDigit(curr32)) {
        _toNext();
        single_quad_u8str += static_cast<char>(curr32);
      } else {
        // The single-quad terminated prematurely, emit everything in ASCII.
        state = State::End;
        _emitCodeUnit(PPCodeUnit::createASCIIChar('\\'));
        _emitCodeUnit(PPCodeUnit::createASCIIChar('u'));
        for (const auto ch: single_quad_u8str)
          _emitCodeUnit(PPCodeUnit::createASCIIChar(ch));
      }
    }

    else if (state == State::DoubleQuad) {
      fprintf(stderr,"State::DoubleQuad\n");
      if (double_quad_u8str.length() == 8) {
        // Emit the universal-character-name the hex-quad is filled.
        state = State::End;
        const char32_t value = static_cast<char32_t>(std::stoull(double_quad_u8str, nullptr, 16));
        _emitCodeUnit(PPCodeUnit::createUniversalCharacterName(value, std::string("\\U") + double_quad_u8str));
      } else if (double_quad_u8str.length() > 8) {
        // Impossible to reach this state given the structure of this DFA.
        state = State::Error;
        _setError(R"(PPCodeUnitStream reached a seemingly impossible internal state.)");
      } else if (PPCodePointCheck::isHexadecimalDigit(curr32)) {
        _toNext();
        double_quad_u8str += static_cast<char>(curr32);
      } else {
        // The single-quad terminated prematurely, emit everything in ASCII.
        state = State::End;
        _emitCodeUnit(PPCodeUnit::createASCIIChar('\\'));
        _emitCodeUnit(PPCodeUnit::createASCIIChar('U'));
        for (const auto ch: double_quad_u8str)
          _emitCodeUnit(PPCodeUnit::createASCIIChar(ch));
      }
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
