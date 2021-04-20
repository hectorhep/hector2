#include "Hector/Elements/ApertureType.h"

namespace hector {
  std::ostream& operator<<(std::ostream& os, const aperture::Type& type) {
    switch (type) {
      case aperture::anInvalidAperture:
        os << "invalid";
        break;
      case aperture::aRectangularAperture:
        os << "rectangular";
        break;
      case aperture::aCircularAperture:
        os << "circular";
        break;
      case aperture::anEllipticAperture:
        os << "elliptic";
        break;
      case aperture::aRectEllipticAperture:
        os << "rect-elliptic";
        break;
      case aperture::aRectCircularAperture:
        os << "rect-circular";
        break;
      case aperture::aRaceTrackAperture:
        os << "race-track";
        break;
      case aperture::anOctagonalAperture:
        os << "octagonal";
        break;
    }
    return os;
  }
}  // namespace hector
