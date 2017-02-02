#ifndef Beamline_BeamProducer
#define Beamline_BeamProducer

#include "propagator/Particle.h"

#include <CLHEP/Random/RandGauss.h>
#include <vector>

namespace Hector
{
  /// Generator for beam of particles
  namespace BeamProducer
  {
    /// Beam of particles to scan the optics following the x axis
    Particles Xscanner( const size_t& num_part, float e_ini, float x_min, float x_max, float y=0., float s_ini=0. );
    /// Beam of particles to scan the optics following the y axis
    Particles Yscanner( const size_t& num_part, float e_ini, float y_min, float y_max, float x=0., float s_ini=0. );
    /// Beam of particles to scan the optics with respect to the x angle
    Particles TXscanner( const size_t& num_part, float e_ini, float tx_min, float tx_max, float ty=0., float s_ini=0. );
    /// Beam of particles to scan the optics with respect to the y angle
    Particles TYscanner( const size_t& num_part, float e_ini, float ty_min, float ty_max, float tx=0., float s_ini=0. );
    /// Beam of particles with gaussian s, x, y, Tx, Ty and energy distributions
    Particles gaussianParticleGun( const size_t& num_part, float e_min, float e_max,
                                   float s_min=0., float s_max=0., float x_min=0., float x_max=0., float y_min=0., float y_max=0.,
                                   float tx_min=-CLHEP::pi/2., float tx_max=CLHEP::pi/2., float ty_min=-CLHEP::pi/2., float ty_max=CLHEP::pi/2.,
                                   float mass=Constants::beam_particles_mass, float charge=Constants::beam_particles_charge );
    /// Beam of particles with flat s, x, y, Tx, Ty and energy distributions
    Particles flatParticleGun( const size_t& num_part, float e_min, float e_max,
                               float s_min=0., float s_max=0., float x_min=0., float x_max=0., float y_min=0., float y_max=0.,
                               float tx_min=-CLHEP::pi/2., float tx_max=CLHEP::pi/2., float ty_min=-CLHEP::pi/2., float ty_max=CLHEP::pi/2.,
                               float mass=Constants::beam_particles_mass, float charge=Constants::beam_particles_charge );

  }
}

#endif
