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

#include "Hector/Elements/Drift.h"
#include "Hector/Elements/Kicker.h"
#include "Hector/Parameters.h"
#include "Hector/Utils/StateVector.h"

namespace hector {
  namespace element {
    Matrix HorizontalKicker::matrix(double eloss, double mp, int qp) const {
      Matrix mat = Drift::genericMatrix(length_);

      if (!Parameters::get()->enableKickers())
        return mat;

      const double ke = -fieldStrength(eloss, mp, qp);
      if (ke == 0.)
        return mat;

      mat(StateVector::X, StateVector::K) = length_ * tan(ke) * 0.5;
      mat(StateVector::TX, StateVector::K) = ke;
      return mat;
    }

    Matrix VerticalKicker::matrix(double eloss, double mp, int qp) const {
      Matrix mat = Drift::genericMatrix(length_);

      if (!Parameters::get()->enableKickers())
        return mat;

      const double ke = -fieldStrength(eloss, mp, qp);
      if (ke == 0.)
        return mat;

      mat(StateVector::Y, StateVector::K) = length_ * tan(ke) * 0.5;
      mat(StateVector::TY, StateVector::K) = ke;
      return mat;
    }
  }  // namespace element
}  // namespace hector
