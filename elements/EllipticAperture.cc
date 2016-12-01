#include "EllipticAperture.h"

namespace Hector
{
  namespace Aperture
  {
    EllipticAperture::EllipticAperture( float ell_length, float ell_height, const CLHEP::Hep2Vector& pos ) :
      ApertureBase( ApertureBase::Elliptic, pos, ell_length, ell_height, 0., 0. )
    {
    }

    EllipticAperture::~EllipticAperture()
    {}

    bool
    EllipticAperture::contains( const CLHEP::Hep2Vector& pos ) const
    {
      const CLHEP::Hep2Vector vec( ( pos.x()-pos_.x() )/x1_, ( pos.y()-pos_.y() )/x2_ );
      return ( vec.mag2()<1. );
    }
  }
}
