#ifndef Elements_RectangularAperture
#define Elements_RectangularAperture

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

        RectangularAperture* clone() const { return new RectangularAperture( *this ); }

        bool contains( const CLHEP::Hep2Vector& ) const;

    };
  }
}

#endif
