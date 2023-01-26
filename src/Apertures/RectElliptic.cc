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

#include "Hector/Apertures/RectElliptic.h"

namespace hector {
  namespace aperture {
    RectElliptic::RectElliptic(
        double rect_size_x, double rect_size_y, double ell_size_x, double ell_size_y, const TwoVector& pos)
        : Aperture(aRectEllipticAperture, pos, {{rect_size_x, rect_size_y, ell_size_x, ell_size_y}}) {}

    bool RectElliptic::contains(const TwoVector& pos) const {
      const TwoVector vec1(pos - pos_), vec2(vec1.x() / p(2), vec1.y() / p(3));
      return ((fabs(vec1.x()) < p(0)) && (fabs(vec1.y()) < p(1))  // rectangular part
              && (vec2.norm() < 1.));                             // elliptic part
    }

    TwoVector RectElliptic::limits() const {  //FIXME
      return TwoVector(std::min(p(0), p(2)), std::min(p(1), p(3)));
    }
  }  // namespace aperture
}  // namespace hector
