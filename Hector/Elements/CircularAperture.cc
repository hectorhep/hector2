#include "CircularAperture.h"

namespace Hector
{
  namespace Aperture
  {
    CircularAperture::CircularAperture( float radius, const TwoVector& pos ) :
      EllipticAperture( radius, radius, pos )
    {
    }

    CircularAperture::~CircularAperture()
    {}
  }
}
