#include "RectangularAperture.h"

namespace Element
{
  RectangularAperture::RectangularAperture( float rect_length, float rect_height, const CLHEP::Hep2Vector& pos ) :
    ApertureBase( ApertureBase::Rectangular, pos, rect_length, rect_height, 0., 0. )
  {
  }

  RectangularAperture::~RectangularAperture()
  {}

  bool
  RectangularAperture::contains( const CLHEP::Hep2Vector& pos ) const
  {
    const CLHEP::Hep2Vector vec( pos-pos_ );
    return ( fabs( vec.x() )<x1_ and fabs( vec.y() )<x2_ );
  }
}
