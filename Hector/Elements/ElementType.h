#ifndef Hector_Elements_ElementType_h
#define Hector_Elements_ElementType_h

#include <iostream>

namespace hector {
  namespace element {
    /// List of types allowed for an element
    enum Type {
      anInvalidElement = -1,   ///< Invalid element
      aMarker,                 ///< Simple s-marker
      aDrift,                  ///< Drift element
      aMonitor,                ///< Monitoring element
      aRectangularDipole,      ///< Rectangular-type dipole
      aSectorDipole,           ///< Sector-type dipole
      aGenericQuadrupole,      ///< Generic (vertical or horizontal) quadrupole
      aVerticalQuadrupole,     ///< Vertical-type quadrupole
      anHorizontalQuadrupole,  ///< Horizontal-type quadrupole
      aSextupole,              ///< Generic sextupole
      aMultipole,              ///< Generic multipole
      aVerticalKicker,         ///< Vertical-type kicker
      anHorizontalKicker,      ///< Horizontal-type kicker
      aRectangularCollimator,  ///< Rectangular-type collimator
      anEllipticalCollimator,  ///< Rectangular-type collimator
      aCircularCollimator,     ///< Circular-type collimator
      aCollimator,             ///< Generic collimator
      //RomanPot, InteractionPoint,
      aPlaceholder,  ///< A placeholder element
      anInstrument,  ///< A measurement apparatus
      aSolenoid      ///< Solenoid
    };
  }  // namespace element
  /// Human-readable printout of a element type
  std::ostream& operator<<(std::ostream&, const element::Type&);
}  // namespace hector

#endif
