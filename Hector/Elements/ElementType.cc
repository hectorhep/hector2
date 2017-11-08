#include "ElementType.h"

namespace Hector
{
  namespace Element
  {
    /// Human-readable printout of a beamline element type
    std::ostream&
    operator<<( std::ostream& os, const Type& type )
    {
      switch ( type ) {
        case anInvalidElement:       return os << "invalid";
        case aMarker:                return os << "marker";
        case aDrift:                 return os << "drift";
        case aMonitor:               return os << "monitor";
        case aRectangularDipole:     return os << "rect.dipole";
        case aSectorDipole:          return os << "sector dipole";
        case aGenericQuadrupole:     return os << "quadrupole";
        case anHorizontalQuadrupole: return os << "hor.quadrupole";
        case aVerticalQuadrupole:    return os << "vert.quadrupole";
        case aSextupole:             return os << "sextupole";
        case aMultipole:             return os << "multipole";
        case aVerticalKicker:        return os << "vertic.kicker";
        case anHorizontalKicker:     return os << "horiz.kicker";
        case aRectangularCollimator: return os << "rect.collimator";
        case anEllipticalCollimator: return os << "ellip.collimator";
        case aCircularCollimator:    return os << "circular collimator";
        case aPlaceholder:           return os << "placeholder";
        case anInstrument:           return os << "instrument";
        case aSolenoid:              return os << "solenoid";
      }
      return os;
    }
  }
}
