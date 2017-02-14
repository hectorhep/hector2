#ifndef Elements_RectangularCollimator_h
#define Elements_RectangularCollimator_h

#include "Drift.h"

namespace Hector
{
  namespace Element
  {
    /// Rectangular collimator object builder
    class RectangularCollimator : public Drift
    {
      public:
        /// Class constructor
        RectangularCollimator( const std::string& name ) : Drift( name, ElementBase::RectangularCollimator ) {}

        RectangularCollimator* clone() const { return new RectangularCollimator( *this ); }
        //void computeMatrix( float, float, int );

      private:

    };
  }
}

#endif
