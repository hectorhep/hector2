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
        RectEllipticAperture( double, double, double, double, const TwoVector& pos = TwoVector() );
        ~RectEllipticAperture();

        std::shared_ptr<ApertureBase> clone() const override { return std::make_shared<RectEllipticAperture>( *this ); }

        bool contains( const TwoVector& ) const override;
        TwoVector limits() const override;
    };
  }
}

#endif
