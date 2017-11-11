#include "ApertureType.h"

namespace Hector
{
  namespace Aperture
  {
    std::ostream&
    operator<<( std::ostream& os, const Type& type )
    {
      switch ( type ) {
        case anInvalidAperture:     os << "invalid"; break;
        case aRectangularAperture:  os << "rectangular"; break;
        case aCircularAperture:     os << "circular"; break;
        case anEllipticAperture:    os << "elliptic"; break;
        case aRectEllipticAperture: os << "rect-elliptic"; break;
        case aRectCircularAperture: os << "rect-circular"; break;
        case aRaceTrackAperture:    os << "race-track"; break;
        case anOctagonalAperture:   os << "octagonal"; break;
      }
      return os;
    }
  }
}
