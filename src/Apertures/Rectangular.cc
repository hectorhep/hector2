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

#include "Hector/Apertures/Rectangular.h"

namespace hector {
  namespace aperture {
    Rectangular::Rectangular(double rect_length, double rect_height, const TwoVector& pos)
        : Aperture(aRectangularAperture, pos, {{rect_length, rect_height, 0., 0.}}) {}

    bool Rectangular::contains(const TwoVector& pos) const {
      const TwoVector vec(pos - pos_);
      return (fabs(vec.x()) < p(0) && fabs(vec.y()) < p(1));
    }

    TwoVector Rectangular::limits() const { return TwoVector(p(0), p(1)); }
  }  // namespace aperture
}  // namespace hector
