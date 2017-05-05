#include "EllipticAperture.h"

namespace Hector
{
  namespace Aperture
  {
    EllipticAperture::EllipticAperture( float ell_length, float ell_height, const CLHEP::Hep2Vector& pos ) :
      ApertureBase( anEllipticAperture, pos, std::vector<float>({ ell_length, ell_height, 0.f, 0.f }) )
    {}

    EllipticAperture::~EllipticAperture()
    {}

    bool
    EllipticAperture::contains( const CLHEP::Hep2Vector& pos ) const
    {
      const CLHEP::Hep2Vector vec( ( pos.x()-pos_.x() )/p( 0 ), ( pos.y()-pos_.y() )/p( 1 ) );
      return ( vec.mag2()<1. );
    }

    CLHEP::Hep2Vector
    EllipticAperture::limits() const
    {
      return CLHEP::Hep2Vector( p( 0 ), p( 1 ) );
    }
  }
}
