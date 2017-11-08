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

        std::shared_ptr<ApertureBase> clone() const override { return std::make_shared<EllipticAperture>( *this ); }

        bool contains( const CLHEP::Hep2Vector& ) const override;
        CLHEP::Hep2Vector limits() const override;
    };
  }
}

#endif
