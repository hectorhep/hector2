#ifndef Elements_RectangularCollimator_h
#define Elements_RectangularCollimator_h

#include "Drift.h"

namespace Hector
{
  namespace Element
  {
    class RectangularCollimator : public Drift
    {
      public:
        RectangularCollimator( const std::string& name ) : Drift( ElementBase::RectangularCollimator, name ) {}

        RectangularCollimator* clone() const {
          RectangularCollimator* out = new RectangularCollimator( *this );
          if ( aperture_ ) out->setAperture( aperture_->clone() );
          return out;
        }
        //void computeMatrix( float, float, int );

      private:

    };
  }
}

#endif
