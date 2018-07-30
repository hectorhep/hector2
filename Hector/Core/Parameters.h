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
      /// Retrieve this (unique) singleton
      static std::shared_ptr<Parameters> get();
      /// Build a new set of parameters
      Parameters();

      /// Energy of the primary particles in the beam (in GeV)
      float beamEnergy() const { return beam_energy_; }
      /// Set the primary particles energy (in GeV)
      void setBeamEnergy( float be ) { beam_energy_ = be; }

      /// Mass of the primary particles in the beam (in GeV/c2)
      float beamParticlesMass() const { return beam_particles_mass_; }
      /// Set the primary particles mass (in GeV/c2)
      void setBeamParticlesMass( float m ) { beam_particles_mass_ = m; }

      /// Electric charge of the primary particles in the beam (in e)
      int beamParticlesCharge() const { return beam_particles_charge_; }
      /// Set the primary particles electric charge (in e)
      void setBeamParticlesCharge( int q ) { beam_particles_charge_ = q; }

      /// Exceptions verbosity
      ExceptionType loggingThreshold() const { return logging_threshold_; }
      /// Set the exceptions verbosity
      void setLoggingThreshold( const ExceptionType& type ) { logging_threshold_ = type; }

      /// Do we use the relative energy loss in the path computation through elements?
      bool useRelativeEnergy() const { return use_relative_energy_; }
      /// Use the relative energy loss?
      void setUseRelativeEnergy( bool rel ) { use_relative_energy_ = rel; }

      /// Are the elements overlaps to be corrected inside a beamline
      bool correctBeamlineOverlaps() const { return correct_beamline_overlaps_; }
      /// Are the elements overlaps to be corrected inside a beamline
      void setCorrectBeamlineOverlaps( bool ov ) { correct_beamline_overlaps_ = ov; }

      /// Account for the acceptance of each single element?
      bool computeApertureAcceptance() const { return compute_aperture_acceptance_; }
      /// Account for the acceptance of each single element?
      void setComputeApertureAcceptance( bool aper ) { compute_aperture_acceptance_ = aper; }

      bool enableKickers() const { return enable_kickers_; }
      void setEnableKickers( bool kck ) { enable_kickers_ = kck; }

      bool enableDipoles() const { return enable_dipoles_; }
      void setEnableDipoles( bool dip ) { enable_dipoles_ = dip; }

    private:
      float beam_energy_;
      float beam_particles_mass_;
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
