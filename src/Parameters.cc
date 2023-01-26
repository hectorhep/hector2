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

#include "Hector/Parameters.h"

namespace hector {
  Parameters::Parameters()
      : beam_energy_(6500.),                // in GeV
        beam_particles_mass_(0.938272046),  // in GeV
        beam_particles_charge_(+1),
        logging_threshold_(ExceptionType::info),
        use_relative_energy_(true),
        correct_beamline_overlaps_(true),
        compute_aperture_acceptance_(true),
        enable_kickers_(false),
        enable_dipoles_(true) {}

  std::shared_ptr<Parameters> Parameters::get() {
    static std::shared_ptr<Parameters> params(new Parameters);
    return params;
  }
}  // namespace hector
