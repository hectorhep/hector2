#include "ElementType.h"

/// Human-readable printout of a beamline element type
std::ostream&
operator<<( std::ostream& os, const Hector::Element::Type& type )
{
  switch ( type ) {
    case Hector::Element::anInvalidElement:       return os << "invalid";
    case Hector::Element::aMarker:                return os << "marker";
    case Hector::Element::aDrift:                 return os << "drift";
    case Hector::Element::aMonitor:               return os << "monitor";
    case Hector::Element::aRectangularDipole:     return os << "rect.dipole";
    case Hector::Element::aSectorDipole:          return os << "sector dipole";
    case Hector::Element::aGenericQuadrupole:     return os << "quadrupole";
    case Hector::Element::anHorizontalQuadrupole: return os << "hor.quadrupole";
    case Hector::Element::aVerticalQuadrupole:    return os << "vert.quadrupole";
    case Hector::Element::aSextupole:             return os << "sextupole";
    case Hector::Element::aMultipole:             return os << "multipole";
    case Hector::Element::aVerticalKicker:        return os << "vertic.kicker";
    case Hector::Element::anHorizontalKicker:     return os << "horiz.kicker";
    case Hector::Element::aRectangularCollimator: return os << "rect.collimator";
    case Hector::Element::anEllipticalCollimator: return os << "ellip.collimator";
    case Hector::Element::aCircularCollimator:    return os << "circular collimator";
    case Hector::Element::aPlaceholder:           return os << "placeholder";
    case Hector::Element::anInstrument:           return os << "instrument";
    case Hector::Element::aSolenoid:              return os << "solenoid";
  }
  return os;
}
