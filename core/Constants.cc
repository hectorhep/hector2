#include "Constants.h"

namespace Hector
{
  float Constants::beam_energy = 6500.; // in GeV
  float Constants::beam_particles_mass = 0.938272046; // in GeV
  float Constants::crossing_angle = 285.0e-6; // in rad
  int Constants::beam_particles_charge = +1;
  bool Constants::use_relative_energy = true;
  bool Constants::correct_beamline_overlaps = true;
}
