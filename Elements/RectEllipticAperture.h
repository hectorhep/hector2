#ifndef Hector_Elements_RectEllipticAperture
#define Hector_Elements_RectEllipticAperture

#include "ApertureBase.h"

namespace Hector
{
  namespace Aperture
  {
    /// Rectangular/elliptic shape aperture
    class RectEllipticAperture : public ApertureBase
    {
      public:
        /// Class constructor
        RectEllipticAperture( float, float, float, float, const CLHEP::Hep2Vector& pos=CLHEP::Hep2Vector() );
        ~RectEllipticAperture();

        RectEllipticAperture* clone() const { return new RectEllipticAperture( *this ); }

        bool contains( const CLHEP::Hep2Vector& ) const;
        CLHEP::Hep2Vector limits() const;

    };
  }
}

#endif
