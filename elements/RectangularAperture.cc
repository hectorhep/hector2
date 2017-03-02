#include "RectangularAperture.h"

namespace Hector
{
  namespace Aperture
  {
    RectangularAperture::RectangularAperture( float rect_length, float rect_height, const CLHEP::Hep2Vector& pos ) :
      ApertureBase( aRectangularAperture, pos, std::vector<float>({ rect_length, rect_height, 0., 0. }) )
    {}

    RectangularAperture::~RectangularAperture()
    {}

    bool
    RectangularAperture::contains( const CLHEP::Hep2Vector& pos ) const
    {
      const CLHEP::Hep2Vector vec( pos-pos_ );
      return ( fabs( vec.x() )<p( 0 ) and fabs( vec.y() )<p( 1 ) );
    }
  }
}
