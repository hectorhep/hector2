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
      float beamEnergy() const { return beam_energy_; }
      void setBeamEnergy( float be ) { beam_energy_ = be; }

      /// Mass of the primary particles in the beam (in GeV/c2)
      float beamParticlesMass() const { return beam_particles_mass_; }
      void setBeamParticlesMass( float m ) { beam_particles_mass_ = m; }

      /// Horizontal crossing angle of the particles beams (in rad)
      float crossingAngleX() const { return crossing_angle_x_; }
      void setCrossingAngleX( float tx ) { crossing_angle_x_ = tx; }

      /// Vertical crossing angle of the particles beams (in rad)
      float crossingAngleY() const { return crossing_angle_y_; }
      void setCrossingAngleY( float ty ) { crossing_angle_y_ = ty; }

      /// Electric charge of the primary particles in the beam (in e)
      int beamParticlesCharge() const { return beam_particles_charge_; }
      void setBeamParticlesCharge( int q ) { beam_particles_charge_ = q; }

      ExceptionType loggingThreshold() const { return logging_threshold_; }
      void setLoggingThreshold( const ExceptionType& type ) { logging_threshold_ = type; }

      /// Do we use the relative energy loss in the path computation through elements?
      bool useRelativeEnergy() const { return use_relative_energy_; }
      void setUseRelativeEnergy( bool rel ) { use_relative_energy_ = rel; }

      /// Are the elements overlaps to be corrected inside a beamline
      bool correctBeamlineOverlaps() const { return correct_beamline_overlaps_; }
      void setCorrectBeamlineOverlaps( bool ov ) { correct_beamline_overlaps_ = ov; }

      bool computeApertureAcceptance() const { return compute_aperture_acceptance_; }
      void setComputeApertureAcceptance( bool aper ) { compute_aperture_acceptance_ = aper; }

      bool enableKickers() const { return enable_kickers_; }
      void setEnableKickers( bool kck ) { enable_kickers_ = kck; }

      bool enableDipoles() const { return enable_dipoles_; }
      void setEnableDipoles( bool dip ) { enable_dipoles_ = dip; }

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
