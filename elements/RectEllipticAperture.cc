#include "RectEllipticAperture.h"

namespace Hector
{
  namespace Aperture
  {
    RectEllipticAperture::RectEllipticAperture( float rect_size_x, float rect_size_y, float ell_size_x, float ell_size_y, const CLHEP::Hep2Vector& pos ) :
      ApertureBase( aRectEllipticAperture, pos, std::vector<float>({ rect_size_x, rect_size_y, ell_size_x, ell_size_y }) )
    {}

    RectEllipticAperture::~RectEllipticAperture()
    {}

    bool
    RectEllipticAperture::contains( const CLHEP::Hep2Vector& pos ) const
    {
      const CLHEP::Hep2Vector vec1( pos-pos_ ),
                              vec2( vec1.x()/p( 2 ), vec1.y()/p( 3 ) );
      return ( ( fabs( vec1.x() )<p( 0 ) ) and ( fabs( vec1.y() )<p( 1 ) ) // rectangular part
           and ( vec2.mag2()<1. ) ); // elliptic part
    }

    void
    RectEllipticAperture::limits( float& x, float& y ) const
    { //FIXME
      x = std::min( p( 0 ), p( 2 ) );
      y = std::min( p( 1 ), p( 3 ) );
    }
  }
}
