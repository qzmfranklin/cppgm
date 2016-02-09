#ifndef PPCodeUnitStreamIfc_h
#define PPCodeUnitStreamIfc_h

class PPCodeUnitStreamIfc {
public:
  virtual bool isEmpty() const = 0;
  virtual const std::shared_ptr<PPCodeUnit> getCodeUnit() const = 0;
  virtual void toNext() = 0;
};

#endif /* end of include guard */
