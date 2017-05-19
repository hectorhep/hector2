#include "Parameters.h"

namespace Hector
{
  float Parameters::beam_energy = 6500.; // in GeV
  float Parameters::beam_particles_mass = 0.938272046; // in GeV
  float Parameters::crossing_angle_x = 370.0e-6; // in rad
  float Parameters::crossing_angle_y = 0.; // in rad

  int Parameters::beam_particles_charge = +1;

  ExceptionType Parameters::logging_threshold = Info;

  bool Parameters::use_relative_energy = false;
  bool Parameters::correct_beamline_overlaps = false;
  bool Parameters::compute_aperture_acceptance = true;
  bool Parameters::enable_kickers = false;
  bool Parameters::enable_dipoles = true;
}
