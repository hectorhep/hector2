#ifndef Hector_Core_Parameters_h
#define Hector_Core_Parameters_h

#include "ExceptionType.h"

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
      /// Energy of the primary particles in the beam (in GeV)
      static float beamEnergy() { return beam_energy_; }
      static void setBeamEnergy( float energy ) { beam_energy_ = energy; }
    
      /// Mass of the primary particles in the beam (in GeV/c2)
      static float beamParticlesMass() { return beam_particles_mass_; }
      static void setBeamParticlesMass( float mass ) { beam_particles_mass_ = mass; }

      /// Horizontal crossing angle of the particles beams (in rad)
      static float crossingAngleX() { return crossing_angle_x_; }
      static void setCrossingAngleX( float xangle ) { crossing_angle_x_ = xangle; }

      /// Vertical crossing angle of the particles beams (in rad)
      static float crossingAngleY() { return crossing_angle_y_; }
      static void setCrossingAngleY( float xangle ) { crossing_angle_y_ = xangle; }

      /// Electric charge of the primary particles in the beam (in e)
      static int beamParticlesCharge() { return beam_particles_charge_; }
      static void setBeamParticlesCharge( int q ) { beam_particles_charge_ = q; }

      static ExceptionType loggingThreshold() { return logging_threshold_; }
      static void setLoggingThreshold( const ExceptionType& thr ) { logging_threshold_ = thr; }

      /// Do we use the relative energy loss in the path computation through elements?
      static bool useRelativeEnergy() { return use_relative_energy_; }
      static void setUseRelativeEnergy( bool rel ) { use_relative_energy_ = rel; }

      /// Are the elements overlaps to be corrected inside a beamline
      static bool correctBeamlineOverlaps() { return correct_beamline_overlaps_; }
      static void setCorrectBeamlineOverlaps( bool ov ) { correct_beamline_overlaps_ = ov; }

      static bool computeApertureAcceptance() { return compute_aperture_acceptance_; }
      static void setComputeApertureAcceptance( bool acc ) { compute_aperture_acceptance_ = acc; }

      static bool enableKickers() { return enable_kickers_; }
      static void setEnableKickers( bool en ) { enable_kickers_ = en; }

      static bool enableDipoles() { return enable_dipoles_; }
      static void setEnableDipoles( bool en ) { enable_dipoles_ = en; }

    private:
      static float beam_energy_;
      static float beam_particles_mass_;
      static float crossing_angle_x_;
      static float crossing_angle_y_;
      static int beam_particles_charge_;
      static ExceptionType logging_threshold_;
      static bool use_relative_energy_;
      static bool correct_beamline_overlaps_;
      static bool compute_aperture_acceptance_;
      static bool enable_kickers_;
      static bool enable_dipoles_;
  };
}

#endif
