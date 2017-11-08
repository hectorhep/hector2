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

        std::shared_ptr<ApertureBase> clone() const override { return std::make_shared<RectEllipticAperture>( *this ); }

        bool contains( const CLHEP::Hep2Vector& ) const override;
        CLHEP::Hep2Vector limits() const override;
    };
  }
}

#endif
