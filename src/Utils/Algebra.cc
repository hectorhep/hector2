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

#include "Hector/Utils/Algebra.h"

namespace hector {
  namespace math {
    /// Compute the tangent of both the components of a 2-vector
    TwoVector tan2(const TwoVector& ang) {
      const double x = ang.x(), y = ang.y();
      return TwoVector(tan(x), tan(y));
    }

    /// Compute the arc-tangent of both the components of a 2-vector
    TwoVector atan2(const TwoVector& ang) {
      const double x = ang.x(), y = ang.y();
      return TwoVector(atan(x), atan(y));
    }
  }  // namespace math

  double LorentzVector::operator*(const LorentzVector& oth) {
    return w() * oth.w() - x() * oth.x() - y() * oth.y() - z() * oth.z();
  }

  double LorentzVector::m() const {
    const double m2 = LorentzVector(*this) * (*this);
    if (m2 < 0.)
      return -std::sqrt(-m2);
    return std::sqrt(m2);
  }
}  // namespace hector
