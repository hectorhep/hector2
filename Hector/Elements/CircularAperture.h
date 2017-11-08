#ifndef Hector_Elements_CircularAperture_h
#define Hector_Elements_CircularAperture_h

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

        std::shared_ptr<ApertureBase> clone() const override { return std::make_shared<CircularAperture>( *this ); }
    };
  }
}

#endif
