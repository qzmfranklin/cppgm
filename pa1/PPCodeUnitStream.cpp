#include "PPCodePointCheck.h"
#include "PPCodeUnitStream.h"
#include <string>


PPCodeUnitStream::PPCodeUnitStream(std::shared_ptr<UTF32StreamIfc> u32stream):
  _u32stream(u32stream), _curr(nullptr)
{
  this->toNext();
}

bool PPCodeUnitStream::isEmpty() const
{
  return _u32stream->isEmpty()  &&  (_curr == nullptr);
}

const std::shared_ptr<PPCodeUnit> PPCodeUnitStream::getCodeUnit() const
{
  return _curr;
}

std::string PPCodeUnitStream::getErrorMessage() const
{
  return _errorMessage;
}

void PPCodeUnitStream::_setError(const std::string &&u8str)
{
  fprintf(stderr,"%s\n", u8str.c_str());
  _errorMessage = std::move(u8str);
}

void PPCodeUnitStream::_clearError()
{
  _errorMessage.clear();
}

void PPCodeUnitStream::toNext()
{
  enum class State {
    Start,

    SingleQuad,
    DoubleQuad,
    SingleLineComment,
    MultipleLineCommentStart,
    MultipleLineComment,
    MultipleLineCommentStar,

    End,
    Error
  };

  std::shared_ptr<PPCodeUnit> unit = nullptr;

  std::u32string u32str;
  State state = State::Start;
  _clearError();
  while(state != State::End  &&  state != State::Error) {
    if (_u32stream->isEmpty()) {
      _curr = nullptr;
      break;
    }

    const char32_t curr32 = _u32stream->getChar32();
    _u32stream->toNext();

    switch (state) {

      case State::Start:
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
        } else if (PPCodePointCheck::isBasicSourceCharacter(curr32)) {
          _curr = PPCodeUnit::createCodePoint(static_cast<const char>(curr32));
          state = State::End;
        } else {
          _setError(R"(Not a basic-source-character)");
          state = State::Error;
        }
        break;

      case State::SingleQuad:
        {
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
          _curr = PPCodeUnit::createUniversalCharacterName(value, std::string("\\u") + u8str);
          state = State::End;
        }
        break;

      case State::DoubleQuad:
        {
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
          _curr = PPCodeUnit::createUniversalCharacterName(value, std::string("\\U") + u8str);

          state = State::End;
        }
        break;

      case State::SingleLineComment:
        {
          u32str = U"//";
          u32str += curr32;
          while(1) {
            if (_u32stream->isEmpty()) {
              state = State::Error;
              _setError(R"(Reached end-of-file in single-line-comment)");
              _curr = nullptr;
              break;
            }

            const char32_t ch32 = _u32stream->getChar32();
            if (ch32 == U'\n') {
              _curr = PPCodeUnit::createComment(u32str);
              state = State::End;
              break;
            } else {
              u32str += ch32;
              _u32stream->toNext();
            }
          }
        }
        break;

      case State::MultipleLineCommentStart:
        // *      =>  MultipleLineComment
        // other  =>  Error
        {
          u32str = U"/*";
          u32str += curr32;
          state = State::MultipleLineComment;
        }
        break;

      case State::MultipleLineComment:
        // *      =>  MultipleLineCommentStar
        // other  =>  MultipleLineComment
        {
          u32str += curr32;
          if (curr32 == U'*')
            state = State::MultipleLineCommentStar;
        }
        break;

      case State::MultipleLineCommentStar:
        // *      =>  MultipleLineCommentStar
        // /      =>  End
        // other  =>  MultipleLineComment
        {
          u32str += curr32;

          if (curr32 == U'*') {
            break;
          } else if (curr32 == U'/') {
            state = State::End;
            _curr = PPCodeUnit::createComment(u32str);
          } else {
            state = State::MultipleLineComment;
            continue;
          }
        }
        break;

      case State::End:
        state = State::Error;
        _setError(R"(Reached an impossible state State::End)");
        break;

      case State::Error:
        state = State::Error;
        _setError(R"(Reached an impossible state State::Error)");
        break;
    }

    //fprintf(stderr,"}\n", static_cast<uint32_t>(curr32));
  } // while
}
