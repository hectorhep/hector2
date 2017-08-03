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
      float beamEnergy() { return beam_energy_; }
      void setBeamEnergy( float energy ) { beam_energy_ = energy; }
    
      /// Mass of the primary particles in the beam (in GeV/c2)
      float beamParticlesMass() { return beam_particles_mass_; }
      void setBeamParticlesMass( float mass ) { beam_particles_mass_ = mass; }

      /// Horizontal crossing angle of the particles beams (in rad)
      float crossingAngleX() { return crossing_angle_x_; }
      void setCrossingAngleX( float xangle ) { crossing_angle_x_ = xangle; }

      /// Vertical crossing angle of the particles beams (in rad)
      float crossingAngleY() { return crossing_angle_y_; }
      void setCrossingAngleY( float xangle ) { crossing_angle_y_ = xangle; }

      /// Electric charge of the primary particles in the beam (in e)
      int beamParticlesCharge() { return beam_particles_charge_; }
      void setBeamParticlesCharge( int q ) { beam_particles_charge_ = q; }

      ExceptionType loggingThreshold() { return logging_threshold_; }
      void setLoggingThreshold( const ExceptionType& thr ) { logging_threshold_ = thr; }

      /// Do we use the relative energy loss in the path computation through elements?
      bool useRelativeEnergy() { return use_relative_energy_; }
      void setUseRelativeEnergy( bool rel ) { use_relative_energy_ = rel; }

      /// Are the elements overlaps to be corrected inside a beamline
      bool correctBeamlineOverlaps() { return correct_beamline_overlaps_; }
      void setCorrectBeamlineOverlaps( bool ov ) { correct_beamline_overlaps_ = ov; }

      bool computeApertureAcceptance() { return compute_aperture_acceptance_; }
      void setComputeApertureAcceptance( bool acc ) { compute_aperture_acceptance_ = acc; }

      bool enableKickers() { return enable_kickers_; }
      void setEnableKickers( bool en ) { enable_kickers_ = en; }

      bool enableDipoles() { return enable_dipoles_; }
      void setEnableDipoles( bool en ) { enable_dipoles_ = en; }

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
