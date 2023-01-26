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
#include "Hector/Elements/Quadrupole.h"
#include "Hector/Exception.h"
#include "Hector/Utils/StateVector.h"

namespace hector {
  namespace element {
    Matrix HorizontalQuadrupole::matrix(double eloss, double mp, int qp) const {
      Matrix mat = Drift::genericMatrix(length_);

      const double ke = fieldStrength(eloss, mp, qp);  // should be negative
      if (ke > 0.)
        throw H_ERROR << "Magnetic strength for horizontal quadrupole " << name_ << " should be negative!\n\t"
                      << "Value = " << ke << ".";
      if (ke == 0.) {  // simple drift matrix
        H_DEBUG << "Quadrupole " << name_ << " has no effect. Treating it as a drift.";
        return mat;
      }

      const double sq_k = sqrt(-ke), inv_sq_k = 1. / sq_k;
      const double omega = sq_k * length_;
      const double s_omega = sin(omega), c_omega = cos(omega), sh_omega = sinh(omega), ch_omega = cosh(omega);

      // Focussing Twiss matrix for the horizontal component
      mat(StateVector::X, StateVector::X) = c_omega;
      mat(StateVector::X, StateVector::TX) = s_omega * inv_sq_k;
      mat(StateVector::TX, StateVector::X) = s_omega * (-sq_k);
      mat(StateVector::TX, StateVector::TX) = c_omega;
      // Defocussing Twiss matrix for the vertical component
      mat(StateVector::Y, StateVector::Y) = ch_omega;
      mat(StateVector::Y, StateVector::TY) = sh_omega * inv_sq_k;
      mat(StateVector::TY, StateVector::Y) = sh_omega * sq_k;
      mat(StateVector::TY, StateVector::TY) = ch_omega;
      return mat;
    }

    Matrix VerticalQuadrupole::matrix(double eloss, double mp, int qp) const {
      Matrix mat = Drift::genericMatrix(length_);

      const double ke = fieldStrength(eloss, mp, qp);
      if (ke < 0.)
        throw H_ERROR << "Magnetic strength for vertical quadrupole " << name_ << " should be positive!\n\t"
                      << "Value = " << ke << ".";
      if (ke == 0.) {  // simple drift matrix
        H_DEBUG << "Quadrupole " << name_ << " has no effect. Treating it as a drift.";
        return mat;
      }

      const double sq_k = sqrt(ke), inv_sq_k = 1. / sq_k;
      const double omega = sq_k * length_;
      const double s_omega = sin(omega), c_omega = cos(omega), sh_omega = sinh(omega), ch_omega = cosh(omega);

      // Defocussing Twiss matrix for the horizontal component
      mat(StateVector::X, StateVector::X) = ch_omega;
      mat(StateVector::X, StateVector::TX) = sh_omega * inv_sq_k;
      mat(StateVector::TX, StateVector::X) = sh_omega * sq_k;
      mat(StateVector::TX, StateVector::TX) = ch_omega;
      // Focussing Twiss matrix for the vertical component
      mat(StateVector::Y, StateVector::Y) = c_omega;
      mat(StateVector::Y, StateVector::TY) = s_omega * inv_sq_k;
      mat(StateVector::TY, StateVector::Y) = s_omega * (-sq_k);
      mat(StateVector::TY, StateVector::TY) = c_omega;
      return mat;
    }
  }  // namespace element
}  // namespace hector
