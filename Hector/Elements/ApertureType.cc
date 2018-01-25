#include "Hector/Elements/ApertureType.h"

namespace Hector
{
  std::ostream&
  operator<<( std::ostream& os, const Aperture::Type& type )
  {
    switch ( type ) {
      case Aperture::anInvalidAperture:     os << "invalid"; break;
      case Aperture::aRectangularAperture:  os << "rectangular"; break;
      case Aperture::aCircularAperture:     os << "circular"; break;
      case Aperture::anEllipticAperture:    os << "elliptic"; break;
      case Aperture::aRectEllipticAperture: os << "rect-elliptic"; break;
      case Aperture::aRectCircularAperture: os << "rect-circular"; break;
      case Aperture::aRaceTrackAperture:    os << "race-track"; break;
      case Aperture::anOctagonalAperture:   os << "octagonal"; break;
    }
    return os;
  }
}

