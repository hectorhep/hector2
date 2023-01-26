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

#include "Hector/Apertures/Elliptic.h"

namespace hector {
  namespace aperture {
    Elliptic::Elliptic(double ell_length, double ell_height, const TwoVector& pos)
        : ApertureBase(anEllipticAperture, pos, {{ell_length, ell_height, 0.f, 0.f}}) {}

    Elliptic::~Elliptic() {}

    bool Elliptic::contains(const TwoVector& pos) const {
      const TwoVector vec((pos.x() - pos_.x()) / p(0), (pos.y() - pos_.y()) / p(1));
      return (vec.norm() < 1.);
    }

    TwoVector Elliptic::limits() const { return TwoVector(p(0), p(1)); }
  }  // namespace aperture
}  // namespace hector
