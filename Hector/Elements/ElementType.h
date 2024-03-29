/*
 *  Hector: a beamline propagation tool
 *  Copyright (C) 2016-2023  Laurent Forthomme
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef Hector_Elements_ElementType_h
#define Hector_Elements_ElementType_h

#include <iosfwd>

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
