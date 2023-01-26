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

#include <iostream>

#include "Hector/Apertures/ApertureType.h"

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
