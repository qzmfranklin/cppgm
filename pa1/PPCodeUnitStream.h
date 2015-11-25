#ifndef PPCodeUnitStream_h
#define PPCodeUnitStream_h

#include "PPCodeUnit.h"
#include "UTF32StreamIfc.h"
#include "PPCodeUnitStreamIfc.h"
#include <queue>
#include <string>

class PPCodeUnitStream: public PPCodeUnitStreamIfc {
public:
  PPCodeUnitStream(std::shared_ptr<UTF32StreamIfc>);

  virtual bool isEmpty() const override;
  virtual const std::shared_ptr<PPCodeUnit> getCodeUnit() const override;
  virtual void toNext() override;

  std::string getErrorMessage() const;

private:
  std::shared_ptr<UTF32StreamIfc> _u32stream;

  // Error reporting related stuff.
  void _setError(const std::string&&);
  void _clearError();
  std::string _errorMessage;

  // _pushCodeUnits() parses the input stream and pushes one or more PPCodeUnits
  // to _queue, depending on the content being parsed. Used by toNext() in the
  // following way: The toNext() function simply pops an element from _queue. If
  // _queue is empty after poping an element, toNext() calls _pushCodeUnits() to
  // try replenishing the _queue.
  //
  //   Unless isEmpty() evaluates to true, the _queue always has at least one
  // element outside of the scope of the toNext() function, so that a)
  // getCodeUnit() has a super simple form, and b) the actual parsing of the
  // input stream is delayed as much as possible.
  //
  //   When isEmpty() evaluates to true iff. both the input stream and the
  // _queue are empty.
  void _pushCodeUnits();
  std::queue<std::shared_ptr<PPCodeUnit>> _queue;
};

#endif /* end of include guard */
