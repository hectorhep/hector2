#include "Hector/Elements/ElementType.h"

namespace hector {
  /// Human-readable printout of a beamline element type
  std::ostream& operator<<(std::ostream& os, const element::Type& type) {
    switch (type) {
      case element::anInvalidElement:
        return os << "invalid";
      case element::aMarker:
        return os << "marker";
      case element::aDrift:
        return os << "drift";
      case element::aMonitor:
        return os << "monitor";
      case element::aRectangularDipole:
        return os << "rect.dipole";
      case element::aSectorDipole:
        return os << "sector dipole";
      case element::aGenericQuadrupole:
        return os << "quadrupole";
      case element::anHorizontalQuadrupole:
        return os << "hor.quadrupole";
      case element::aVerticalQuadrupole:
        return os << "vert.quadrupole";
      case element::aSextupole:
        return os << "sextupole";
      case element::aMultipole:
        return os << "multipole";
      case element::aVerticalKicker:
        return os << "vertic.kicker";
      case element::anHorizontalKicker:
        return os << "horiz.kicker";
      case element::aRectangularCollimator:
        return os << "rect.collimator";
      case element::anEllipticalCollimator:
        return os << "ellip.collimator";
      case element::aCircularCollimator:
        return os << "circular collimator";
      case element::aCollimator:
        return os << "collimator";
      case element::aPlaceholder:
        return os << "placeholder";
      case element::anInstrument:
        return os << "instrument";
      case element::aSolenoid:
        return os << "solenoid";
    }
    return os;
  }
}  // namespace hector
