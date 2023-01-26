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

#ifndef Hector_Apertures_ApertureType_h
#define Hector_Apertures_ApertureType_h

#include <iosfwd>

namespace hector {
  /// Collection of apertures to be associated to element objects
  namespace aperture {
    /// List of types allowed for an aperture
    enum Type {
      anInvalidAperture = -1,
      aRectangularAperture,
      anEllipticAperture,
      aCircularAperture,
      aRectEllipticAperture,
      aRectCircularAperture,
      aRaceTrackAperture,
      anOctagonalAperture
    };
  }  // namespace aperture
  /// Human-readable printout of an aperture type
  std::ostream& operator<<(std::ostream&, const aperture::Type&);
}  // namespace hector

#endif
