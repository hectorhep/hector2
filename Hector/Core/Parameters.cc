#include "Parameters.h"

namespace Hector
{
  float Parameters::beam_energy_ = 6500.; // in GeV
  float Parameters::beam_particles_mass_ = 0.938272046; // in GeV
  float Parameters::crossing_angle_x_ = 370.0e-6; // in rad
  float Parameters::crossing_angle_y_ = 0.; // in rad

  int Parameters::beam_particles_charge_ = +1;

  ExceptionType Parameters::logging_threshold_ = Info;

  bool Parameters::use_relative_energy_ = false;
  bool Parameters::correct_beamline_overlaps_ = false;
  bool Parameters::compute_aperture_acceptance_ = true;
  bool Parameters::enable_kickers_ = false;
  bool Parameters::enable_dipoles_ = true;
}
