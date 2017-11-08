#ifndef Hector_Elements_RectangularAperture
#define Hector_Elements_RectangularAperture

#include "ApertureBase.h"

namespace Hector
{
  namespace Aperture
  {
    /// Rectangular shape aperture
    class RectangularAperture : public ApertureBase
    {
      public:
        /// Class constructor
        RectangularAperture( float, float, const CLHEP::Hep2Vector& pos=CLHEP::Hep2Vector() );
        ~RectangularAperture();

        std::shared_ptr<ApertureBase> clone() const override { return std::make_shared<RectangularAperture>( *this ); }

        bool contains( const CLHEP::Hep2Vector& ) const override;
        CLHEP::Hep2Vector limits() const override;
    };
  }
}

#endif
