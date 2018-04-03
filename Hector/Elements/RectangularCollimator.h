#ifndef Hector_Elements_RectangularCollimator_h
#define Hector_Elements_RectangularCollimator_h

#include "Drift.h"

namespace Hector
{
  namespace Element
  {
    /// Rectangular collimator object builder
    /// \note Treated as a simple drift with a finite aperture opening
    class RectangularCollimator : public Drift
    {
      public:
        /// Class constructor
        RectangularCollimator( const std::string& name, double spos = 0., double length = 0. ) :
          Drift( name, aRectangularCollimator, spos, length ) {}

        std::shared_ptr<ElementBase> clone() const override { return std::make_shared<RectangularCollimator>( *this ); }
    };
  }
}

#endif
