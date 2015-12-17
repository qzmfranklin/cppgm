#ifndef PPTokenizerDFA_h
#define PPTokenizerDFA_h

#include "PPCodeUnit.h"
#include "PPCodeUnitStreamIfc.h"
#include "PPToken.h"
#include <memory>
#include <queue>
#include <string>

// The preprocessing tokenizer Deterministic Finite Automaton (DFA) with
// single-symbol-lookahead.
class PPTokenizerDFA {
public:
  PPTokenizerDFA(std::shared_ptr<PPCodeUnitStreamIfc>);

  bool isEmpty() const;
  std::shared_ptr<PPToken> getPPToken() const;
  void toNext();
  std::string getErrorMessage() const;

private:
  std::shared_ptr<PPCodeUnitStreamIfc> _stream;

  void _setError(const std::string&&);
  void _clearError();
  std::string _errorMessage;

  void _pushTokens();
  std::queue<std::shared_ptr<PPToken>> _queue;

  bool _isBeginningOfLine = true;
  bool _isPreprocessingDirective = false;
  bool _isRawStringMode = false;
};

#endif /* end of include guard */
