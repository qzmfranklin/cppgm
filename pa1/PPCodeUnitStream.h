#ifndef PPCodeUnitStream_h
#define PPCodeUnitStream_h

#include "PPCodeUnit.h"
#include "UTF32StreamIfc.h"
#include "PPCodeUnitStreamIfc.h"

class PPCodeUnitStream: public PPCodeUnitStreamIfc {
public:
  PPCodeUnitStream(std::shared_ptr<UTF32StreamIfc>);

  virtual bool isEmpty() const override;
  virtual const std::shared_ptr<PPCodeUnit> getCodeUnit() const override;
  virtual void toNext() override;

  std::string getErrorMessage() const;

private:
  void _setError(const std::string&&);
  void _clearError();
  std::shared_ptr<UTF32StreamIfc> _u32stream;
  std::shared_ptr<PPCodeUnit> _curr;
  std::string _errorMessage;
};

#endif /* end of include guard */
