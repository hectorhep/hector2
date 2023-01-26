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

#ifndef Hector_Utils_Kinematics_h
#define Hector_Utils_Kinematics_h

namespace hector {
  class Particle;
  /// Let the particle emit a photon
  void emitGamma(Particle& part_in, double e_gamma, double q2_gamma, double phi_min, double phi_max);
  /// Convert a particle energy to its momentum loss
  double e_to_xi(double energy, double e0 = -1.);
  /// Convert a particle momentum loss to its energy
  double xi_to_e(double xi, double e0 = -1.);
}  // namespace hector

#endif
