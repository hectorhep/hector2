#include "EllipticAperture.h"

namespace Hector
{
  namespace Aperture
  {
    EllipticAperture::EllipticAperture( double ell_length, double ell_height, const TwoVector& pos ) :
      ApertureBase( anEllipticAperture, pos, { { ell_length, ell_height, 0.f, 0.f } } )
    {}

    EllipticAperture::~EllipticAperture()
    {}

    bool
    EllipticAperture::contains( const TwoVector& pos ) const
    {
      const TwoVector vec( ( pos.x()-pos_.x() )/p( 0 ), ( pos.y()-pos_.y() )/p( 1 ) );
      return ( vec.mag2() < 1. );
    }

    TwoVector
    EllipticAperture::limits() const
    {
      return TwoVector( p( 0 ), p( 1 ) );
    }
  }
}
