#include "RectEllipticAperture.h"

namespace Hector
{
  namespace Aperture
  {
    RectEllipticAperture::RectEllipticAperture( float rect_length, float rect_height, float ell_length, float ell_height, const CLHEP::Hep2Vector& pos ) :
      ApertureBase( ApertureBase::RectElliptic, pos, rect_length, rect_height, ell_length, ell_height )
    {}

    RectEllipticAperture::~RectEllipticAperture()
    {}

    bool
    RectEllipticAperture::contains( const CLHEP::Hep2Vector& pos ) const
    {
      const CLHEP::Hep2Vector vec1( pos-pos_ ),
                              vec2( ( pos.x()-pos_.x() )/p( 2 ), ( pos.y()-pos_.y() )/p( 3 ) );
      return ( ( fabs( vec1.x() )<p( 0 ) ) and ( fabs( vec1.y() )<p( 1 ) ) // rectangular part
           and ( vec2.mag2()<1. ) ); // elliptic part
    }
  }
}
