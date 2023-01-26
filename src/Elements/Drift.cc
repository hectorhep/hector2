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
#include "Hector/Utils/StateVector.h"

namespace hector {
  namespace element {
    Drift::Drift(const std::string& name, double spos, double length) : Element(aDrift, name, spos, length) {}

    Drift::Drift(const std::string& name, const Type& type, double spos, double length)
        : Element(type, name, spos, length) {}

    Matrix Drift::matrix(double, double, int) const { return genericMatrix(length_); }

    Matrix Drift::genericMatrix(double length) {
      Matrix mat = DiagonalMatrix(6, 1);
      mat(StateVector::X, StateVector::TX) = length;
      mat(StateVector::Y, StateVector::TY) = length;
      return mat;
    }
  }  // namespace element
}  // namespace hector
