#include "CircularAperture.h"

namespace Hector
{
  namespace Aperture
  {
    CircularAperture::CircularAperture( float radius, const CLHEP::Hep2Vector& pos ) :
      EllipticAperture( radius, radius, pos )
    {
    }

    CircularAperture::~CircularAperture()
    {}
  }
}
