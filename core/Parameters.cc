#include "Parameters.h"

namespace Hector
{
  float Parameters::beam_energy = 6500.; // in GeV
  float Parameters::beam_particles_mass = 0.938272046; // in GeV
  float Parameters::crossing_angle = 285.0e-6; // in rad
  //float Parameters::crossing_angle = .0e-6; // in rad
  int Parameters::beam_particles_charge = +1;
  bool Parameters::use_relative_energy = false;
  bool Parameters::correct_beamline_overlaps = true;
}
