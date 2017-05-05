#ifndef Hector_Elements_EllipticAperture
#define Hector_Elements_EllipticAperture

#include "ApertureBase.h"

namespace Hector
{
  namespace Aperture
  {
    /// Elliptic shape aperture
    class EllipticAperture : public ApertureBase
    {
      public:
        /// Class constructor
        EllipticAperture( float, float, const CLHEP::Hep2Vector& pos=CLHEP::Hep2Vector() );
        ~EllipticAperture();

        EllipticAperture* clone() const { return new EllipticAperture( *this ); }

        bool contains( const CLHEP::Hep2Vector& ) const;
        void limits( float& lim_x, float& lim_y ) const;

    };
  }
}

#endif
