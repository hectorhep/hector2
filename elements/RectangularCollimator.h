#ifndef Elements_RectangularCollimator_h
#define Elements_RectangularCollimator_h

#include "Drift.h"

namespace Element
{
  class RectangularCollimator : public Drift
  {
    public:
      RectangularCollimator( const std::string& name ) : Drift( ElementBase::RectangularCollimator, name ) {;}
      ~RectangularCollimator() {;}

    private:

  };
}

#endif
