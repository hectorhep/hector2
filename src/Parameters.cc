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
