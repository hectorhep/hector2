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
    /// Lower and upper limits
    typedef std::pair<float, float> limits;
    /// Generic scanner for a given granularity of a parameter
    class LinearScanner
    {
      public:
        /// Class constructor
        LinearScanner( const unsigned short& num_part, float p1_ini, float p1_end, float p2_ini, float p2_end=999., float s_ini=0., float e_ini=0. ) :
          num_part_( num_part ), num_gen_( 0 ),
          p1_( std::pair<float,float>( p1_ini, p1_end ) ), p2_( std::pair<float,float>( p2_ini, p2_end ) ), s_( s_ini, s_ini ),
          e_ini_( e_ini )
        {
          if ( p2_.second==999. ) { p2_.second = p2_.first; }
        }
        /// Get the next event number to scan
        unsigned short next();
        /// Number of particles to generate to perform a full scan
        unsigned short numScanParticles() const { return num_part_; }
        /// Generate a new particle
        virtual Particle shoot() = 0;

      protected:
        /// Number of particles to generate to perform a full scan
        unsigned short num_part_;
        /// Number of particles already generated in the scan
        unsigned short num_gen_;
        /// Lower and upper limits for the first scan parameter
        limits p1_;
        /// Lower and upper limits for the second scan parameter
        limits p2_;
        /// Lower and upper limits for the s coordinate to generate
        limits s_;
        /// Initial beam energy
        float e_ini_;
    };

    /// Beam of particles to scan the optics following the x axis
    class Xscanner : public LinearScanner
    {
      public:
        /// Class constructor
        Xscanner( const unsigned short& num_part, float e_ini, float x_min, float x_max, float y=0., float s_ini=0. ) :
          LinearScanner( num_part, x_min, x_max, y, y, s_ini, e_ini ) {}
        Particle shoot();
    };

    /// Beam of particles to scan the optics following the y axis
    class Yscanner : public LinearScanner
    {
      public:
        Yscanner( const unsigned short& num_part, float e_ini, float y_min, float y_max, float x=0., float s_ini=0. ) :
          LinearScanner( num_part, y_min, y_max, x, x, s_ini, e_ini ) {}
        Particle shoot();
    };

    /// Beam of particles to scan the optics with respect to the x angle
    class TXscanner : public LinearScanner
    {
      public:
        TXscanner( const unsigned short& num_part, float e_ini, float tx_min, float tx_max, float ty=0., float s_ini=0. ) :
          LinearScanner( num_part, tx_min, tx_max, ty, ty, s_ini, e_ini ) {}
        Particle shoot();
    };

    /// Beam of particles to scan the optics with respect to the y angle
    class TYscanner : public LinearScanner
    {
      public:
        TYscanner( const unsigned short& num_part, float e_ini, float ty_min, float ty_max, float tx=0., float s_ini=0. ) :
          LinearScanner( num_part, ty_min, ty_max, tx, tx, s_ini, e_ini ) {}
        Particle shoot();
    };

    /// A generic templated particle gun
    template<class rnd>
    class ParticleGun
    {
      public:
        /// Class constructor
        ParticleGun( float e_min, float e_max, float s_min=0., float s_max=0.,
                     float x_min=0., float x_max=0., float y_min=0., float y_max=0.,
                     float tx_min=-CLHEP::pi/2., float tx_max=CLHEP::pi/2., float ty_min=-CLHEP::pi/2., float ty_max=CLHEP::pi/2.,
                     float mass=Constants::beam_particles_mass, float charge=Constants::beam_particles_charge ) :
          e_( e_min, e_max ), s_( s_min, s_max ),
          x_( x_min, x_max ), y_( y_min, y_max ),
          tx_( tx_min, tx_max ), ty_( ty_min, ty_max ),
          mass_( mass ), charge_( charge ) {}

        /// Generate one particle according to the templated distribution
        Particle shoot() const {
          Particle::StateVector vec;
          const float s = rnd::shoot( s_.first, s_.second ),
                      x = rnd::shoot( x_.first, x_.second ),
                      y = rnd::shoot( y_.first, y_.second ),
                      tx = rnd::shoot( tx_.first, tx_.second ),
                      ty = rnd::shoot( ty_.first, ty_.second ),
                      e = rnd::shoot( e_.first, e_.second );
          vec.setPosition( CLHEP::Hep2Vector( x, y ) );
          vec.setAngles( CLHEP::Hep2Vector( tx, ty ) );
          vec.setEnergy( e );
          vec.setM( mass_ );

          Particle p( vec, s );
          p.setCharge( charge_ );
          return p;
        }

      private:
        limits e_, s_;
        limits x_, y_;
        limits tx_, ty_;
        float mass_, charge_;
    };

    /// Beam of particles with flat s, x, y, Tx, Ty and energy distributions
    typedef ParticleGun<CLHEP::RandFlat> flatParticleGun;
    /// Beam of particles with gaussian s, x, y, Tx, Ty and energy distributions
    typedef ParticleGun<CLHEP::RandGauss> gaussianParticleGun;
  }
}

#endif
