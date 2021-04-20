#include "ElementType.h"

namespace Hector {
  /// Human-readable printout of a beamline element type
  std::ostream& operator<<(std::ostream& os, const Element::Type& type) {
    switch (type) {
      case Element::anInvalidElement:
        return os << "invalid";
      case Element::aMarker:
        return os << "marker";
      case Element::aDrift:
        return os << "drift";
      case Element::aMonitor:
        return os << "monitor";
      case Element::aRectangularDipole:
        return os << "rect.dipole";
      case Element::aSectorDipole:
        return os << "sector dipole";
      case Element::aGenericQuadrupole:
        return os << "quadrupole";
      case Element::anHorizontalQuadrupole:
        return os << "hor.quadrupole";
      case Element::aVerticalQuadrupole:
        return os << "vert.quadrupole";
      case Element::aSextupole:
        return os << "sextupole";
      case Element::aMultipole:
        return os << "multipole";
      case Element::aVerticalKicker:
        return os << "vertic.kicker";
      case Element::anHorizontalKicker:
        return os << "horiz.kicker";
      case Element::aRectangularCollimator:
        return os << "rect.collimator";
      case Element::anEllipticalCollimator:
        return os << "ellip.collimator";
      case Element::aCircularCollimator:
        return os << "circular collimator";
      case Element::aCollimator:
        return os << "collimator";
      case Element::aPlaceholder:
        return os << "placeholder";
      case Element::anInstrument:
        return os << "instrument";
      case Element::aSolenoid:
        return os << "solenoid";
    }
    return os;
  }
}  // namespace Hector
