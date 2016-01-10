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

  const auto _toNext = [this] () { this->_u32stream->toNext(); };
  const auto _emitCodeUnit = [this] (const std::shared_ptr<PPCodeUnit> ptr) { this->_queue.push(ptr); };

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
        _setError(R"(Not a basic-source-character)");
        state = State::Error;
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
      } else if (curr32 == U'U') { // \UXXXXXXXX
        _toNext();
        state = State::DoubleQuad;
      } else if (PPCodePointCheck::isBasicSourceCharacter(curr32)) {
        state = State::End;
        _emitCodeUnit(PPCodeUnit::createASCIIChar('\''));
      } else {
        state = State::Error;
        _setError(R"(Illegal character following \)");
      }
    }

    else if (state == State::SingleQuad) {
      _toNext();
      fprintf(stderr,"State::SingleQuad\n");
      std::string u8str(1, static_cast<char>(curr32));
      for (int i = 0; i < 3; i++) {
        const char32_t ch32 = _u32stream->getChar32();
        _toNext();
        if (PPCodePointCheck::isHexadecimalDigit(ch32)) {
          u8str  += static_cast<char>(ch32);
        } else {
          state = State::Error;
          _setError(R"(Illegal character following \u: )" + std::string(1, static_cast<char>(ch32)));
        }
      }
      const char32_t value = static_cast<char32_t>(std::stoull(u8str, nullptr, 16));
      _emitCodeUnit(PPCodeUnit::createUniversalCharacterName(value, std::string("\\u") + u8str));
      state = State::End;
    }

    else if (state == State::DoubleQuad) {
      _toNext();
      fprintf(stderr,"State::DoubleQuad\n");
      std::string u8str(1, static_cast<char>(curr32));
      for (int i = 0; i < 7; i++) {
        const char32_t ch32 = _u32stream->getChar32();
        _toNext();
        if (PPCodePointCheck::isHexadecimalDigit(ch32)) {
          u8str  += static_cast<char>(ch32);
        } else {
          state = State::Error;
          _setError(R"(Illegal character following \U)" + std::string(1, static_cast<char>(ch32)));
        }
      }
      const char32_t value = static_cast<char32_t>(std::stoull(u8str, nullptr, 16));
      _emitCodeUnit(PPCodeUnit::createUniversalCharacterName(value, std::string("\\U") + u8str));

      state = State::End;
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
