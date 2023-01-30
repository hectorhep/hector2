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

#include "Hector/Elements/Dipole.h"
#include "Hector/Elements/Drift.h"
#include "Hector/Exception.h"
#include "Hector/Utils/StateVector.h"

namespace hector {
  namespace element {
    Matrix SectorDipole::matrix(double eloss, double mp, int qp) const {
      Matrix mat = Drift::genericMatrix(length_);

      if (!Parameters::get().enableDipoles())
        return mat;

      const double ke = fieldStrength(eloss, mp, qp);
      if (ke == 0.) {  // simple drift matrix
        H_DEBUG << "Sector dipole " << name_ << " has no effect. Treating it as a drift.";
        return mat;
      }

      const double radius = 1. / ke;
      const double theta = length_ * ke, s_theta = sin(theta), c_theta = cos(theta);
      const double inv_energy = 1. / Parameters::get().beamEnergy();

      mat(StateVector::X, StateVector::X) = c_theta;
      mat(StateVector::X, StateVector::TX) = s_theta * radius;
      mat(StateVector::TX, StateVector::X) = s_theta * (-ke);
      mat(StateVector::TX, StateVector::TX) = c_theta;
      if (Parameters::get().useRelativeEnergy()) {
        const double simp = 2. * radius * pow(sin(theta * 0.5), 2) * inv_energy;
        // numerically stable version of ( r/E₀ )*( 1-cos θ )
        mat(StateVector::X, StateVector::E) = simp;
        mat(StateVector::TX, StateVector::E) = s_theta * inv_energy;
      }
      return mat;
    }

    Matrix RectangularDipole::matrix(double eloss, double mp, int qp) const {
      Matrix mat = Drift::genericMatrix(length_);

      if (!Parameters::get().enableDipoles())
        return mat;

      const double ke = fieldStrength(eloss, mp, qp);
      if (ke == 0.) {  // simple drift matrix
        H_DEBUG << "Rectangular dipole " << name_ << " has no effect. Treating it as a drift.";
        return mat;
      }

      const double radius = 1. / ke;
      const double theta = length_ * ke, s_theta = sin(theta), c_theta = cos(theta);
      //std::cout << name_ << "|" << eloss << "|" << radius << "|" << ke << "|" << theta << "|" << s_theta << "|" << c_theta << std::endl;
      const double inv_energy = 1. / Parameters::get().beamEnergy();
      // numerically stable version of ( r/E₀ )*( 1-cos θ )
      const double simp = 2. * radius * pow(sin(theta * 0.5), 2) * inv_energy;

      mat(StateVector::X, StateVector::X) = c_theta;
      mat(StateVector::X, StateVector::TX) = s_theta * radius;
      mat(StateVector::TX, StateVector::X) = s_theta * (-ke);
      mat(StateVector::TX, StateVector::TX) = c_theta;
      mat(StateVector::X, StateVector::E) = simp;
      mat(StateVector::TX, StateVector::E) = s_theta * inv_energy;

      if (Parameters::get().useRelativeEnergy()) {
        Matrix ef_matrix = DiagonalMatrix(6, 1);
        const double t_theta_half_ke = ke * tan(theta * 0.5);
        ef_matrix(StateVector::TX, StateVector::X) = +t_theta_half_ke;
        ef_matrix(StateVector::TY, StateVector::Y) = -t_theta_half_ke;
        return ef_matrix * mat * ef_matrix;
      }

      return mat;
    }
  }  // namespace element
}  // namespace hector
