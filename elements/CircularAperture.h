#ifndef Elements_CircularAperture_h
#define Elements_CircularAperture_h

#include "EllipticAperture.h"

namespace Hector
{
  namespace Aperture
  {
    /// Round shape aperture
    class CircularAperture : public EllipticAperture
    {
      public:
        /// Class constructor
        CircularAperture( float, const CLHEP::Hep2Vector& pos=CLHEP::Hep2Vector() );
        ~CircularAperture();

        CircularAperture* clone() const { return new CircularAperture( *this ); }

      private:
    };
  }
}

#endif
