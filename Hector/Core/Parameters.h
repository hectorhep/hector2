#ifndef Hector_Core_Parameters_h
#define Hector_Core_Parameters_h

#include "ExceptionType.h"
#include <memory>

/// Mother of all namespaces/objects in Hector
/// \author Laurent Forthomme <laurent.forthomme@cernSPAMNOT.ch>
/// \date Nov 2016
namespace Hector
{
  /// List of user-parameterisable constants for the run
  /// \note All parameters are to be left untouched once the run is started
  class Parameters
  {
    public:
      static std::shared_ptr<Parameters> get() { return std::shared_ptr<Parameters>( new Parameters ); }
      Parameters();

      /// Energy of the primary particles in the beam (in GeV)
      float& beamEnergy() { return beam_energy_; }
      /// Mass of the primary particles in the beam (in GeV/c2)
      float& beamParticlesMass() { return beam_particles_mass_; }
      /// Horizontal crossing angle of the particles beams (in rad)
      float& crossingAngleX() { return crossing_angle_x_; }
      /// Vertical crossing angle of the particles beams (in rad)
      float& crossingAngleY() { return crossing_angle_y_; }
      /// Electric charge of the primary particles in the beam (in e)
      int& beamParticlesCharge() { return beam_particles_charge_; }
      ExceptionType& loggingThreshold() { return logging_threshold_; }
      /// Do we use the relative energy loss in the path computation through elements?
      bool& useRelativeEnergy() { return use_relative_energy_; }
      /// Are the elements overlaps to be corrected inside a beamline
      bool& correctBeamlineOverlaps() { return correct_beamline_overlaps_; }
      bool& computeApertureAcceptance() { return compute_aperture_acceptance_; }
      bool& enableKickers() { return enable_kickers_; }
      bool& enableDipoles() { return enable_dipoles_; }

    private:

      float beam_energy_;
      float beam_particles_mass_;
      float crossing_angle_x_;
      float crossing_angle_y_;
      int beam_particles_charge_;
      ExceptionType logging_threshold_;
      bool use_relative_energy_;
      bool correct_beamline_overlaps_;
      bool compute_aperture_acceptance_;
      bool enable_kickers_;
      bool enable_dipoles_;
  };
}

#endif
