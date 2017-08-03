#ifndef Hector_Propagator_BeamProducer
#define Hector_Propagator_BeamProducer

#include "Propagator/Particle.h"

#include <CLHEP/Random/RandGauss.h>
#include <vector>

namespace Hector
{
  /// Generator for beam of particles
  namespace BeamProducer
  {
    /// Lower and upper limits
    typedef std::pair<float, float> params;
    /// Generic scanner for a given granularity of a parameter
    class LinearScanner
    {
      public:
        /// Class constructor
        LinearScanner( const unsigned short& num_part, float p1_ini, float p1_end, float p2_ini, float p2_end=999., float e_ini=0., float e_end=0., float s_ini=0. ) :
          num_part_( num_part ), num_gen_( 0 ),
          p1_( p1_ini, p1_end ), p2_( p2_ini, p2_end ),
          e_( e_ini, e_end ), s_( s_ini, s_ini )
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
        params p1_;
        /// Lower and upper limits for the second scan parameter
        params p2_;
        /// Lower and upper limits on the beam energy
        params e_;
        /// Lower and upper limits for the s coordinate to generate
        params s_;
    };

    /// Beam of particles to scan the optics following the x axis
    class Xscanner : public LinearScanner
    {
      public:
        /// Class constructor
        /// \param[in] num_part number of particles to generate for a full scan
        /// \param[in] e_ini initial energy for the particles
        /// \param[in] x_min minimal parameter value
        /// \param[in] x_max maximal parameter value
        /// \param[in] y fixed parameter value
        /// \param[in] s_ini initial s position
        Xscanner( const unsigned short& num_part, float e_ini, float x_min, float x_max, float y=0., float s_ini=0. ) :
          LinearScanner( num_part, x_min, x_max, y, y, e_ini, e_ini, s_ini ) {}
        Particle shoot();
    };

    /// Beam of particles to scan the optics following the y axis
    class Yscanner : public LinearScanner
    {
      public:
        /// Class constructor
        /// \param[in] num_part number of particles to generate for a full scan
        /// \param[in] e_ini initial energy for the particles
        /// \param[in] x fixed parameter value
        /// \param[in] y_min minimal parameter value
        /// \param[in] y_max maximal parameter value
        /// \param[in] s_ini initial s position
        Yscanner( const unsigned short& num_part, float e_ini, float y_min, float y_max, float x=0., float s_ini=0. ) :
          LinearScanner( num_part, y_min, y_max, x, x, e_ini, e_ini, s_ini ) {}
        Particle shoot();
    };

    /// Beam of particles to scan the optics with respect to the x angle
    class TXscanner : public LinearScanner
    {
      public:
        /// Class constructor
        /// \param[in] num_part number of particles to generate for a full scan
        /// \param[in] e_ini initial energy for the particles
        /// \param[in] tx_min minimal parameter value
        /// \param[in] tx_max maximal parameter value
        /// \param[in] ty fixed parameter value
        /// \param[in] s_ini initial s position
        TXscanner( const unsigned short& num_part, float e_ini, float tx_min, float tx_max, float ty=0., float s_ini=0. ) :
          LinearScanner( num_part, tx_min, tx_max, ty, ty, e_ini, e_ini, s_ini ) {}
        Particle shoot();
    };

    /// Beam of particles to scan the optics with respect to the y angle
    class TYscanner : public LinearScanner
    {
      public:
        /// Class constructor
        /// \param[in] num_part number of particles to generate for a full scan
        /// \param[in] e_ini initial energy for the particles
        /// \param[in] tx fixed parameter value
        /// \param[in] ty_min minimal parameter value
        /// \param[in] ty_max maximal parameter value
        /// \param[in] s_ini initial s position
        TYscanner( const unsigned short& num_part, float e_ini, float ty_min, float ty_max, float tx=0., float s_ini=0. ) :
          LinearScanner( num_part, ty_min, ty_max, tx, tx, e_ini, e_ini, s_ini ) {}
        Particle shoot();
    };
    /// Beam of particles to scan the optics with respect to the longitudinal momentum loss
    class Xiscanner : public LinearScanner
    {
      public:
        /// Class constructor
        /// \param[in] num_part number of particles to generate for a full scan
        /// \param[in] xi_min minimal parameter value
        /// \param[in] xi_max maximal parameter value
        /// \param[in] x horizontal particle position
        /// \param[in] y vertical particle position
        /// \param[in] s_ini initial s position
        Xiscanner( const unsigned short& num_part, float xi_min, float xi_max, float x=0., float y=0., float s_ini=0. ) :
          LinearScanner( num_part, x, x, y, y, Parameters::beam_energy*( 1.-xi_min ), Parameters::beam_energy*( 1.-xi_max ), s_ini ) {}
        Particle shoot();
    };

    /// A generic templated particle gun
    template<class T>
    class ParticleGun
    {
      public:
        /// Class constructor
        ParticleGun( float e_min=Parameters::beam_energy, float e_max=Parameters::beam_energy,
                     float s_min=0., float s_max=0.,
                     float x_min=0., float x_max=0., float y_min=0., float y_max=0.,
                     float tx_min=-CLHEP::pi/2., float tx_max=CLHEP::pi/2., float ty_min=-CLHEP::pi/2., float ty_max=CLHEP::pi/2.,
                     float mass=Parameters::beam_particles_mass, float charge=Parameters::beam_particles_charge ) :
          e_( parameters( e_min, e_max ) ), s_( parameters( s_min, s_max ) ),
          x_( parameters( x_min, x_max ) ), y_( parameters( y_min, y_max ) ),
          tx_( parameters( tx_min, tx_max ) ), ty_( parameters( ty_min, ty_max ) ),
          mass_( mass ), charge_( charge ) {}

        /// Generate one particle according to the templated distribution
        Particle shoot() const {
          StateVector vec;
          const float s = T::shoot( s_.first, s_.second ),
                      x = T::shoot( x_.first, x_.second ),
                      y = T::shoot( y_.first, y_.second ),
                      tx = T::shoot( tx_.first, tx_.second ),
                      ty = T::shoot( ty_.first, ty_.second ),
                      e = T::shoot( e_.first, e_.second );
          vec.setPosition( CLHEP::Hep2Vector( x, y ) );
          vec.setAngles( CLHEP::Hep2Vector( tx, ty ) );
          vec.setM( mass_ );
          vec.setEnergy( e );

          Particle p( vec, s );
          p.setCharge( charge_ );
          return p;
        }

        //----- Full beam information

        /// Set the parameters to the initial beam energy distribution
        void setEparams( float e1, float e2 ) { e_ = params( e1, e2 ); }
        /// Set the lower and upper limits to the initial beam energy distribution
        void setElimits( float e1, float e2=-1. ) {
          if ( e2<0 ) e2 = e1; // energies are supposingly positive
          e_ = parameters( e1, e2 );
        }

        /// Set the parameters to the initial longitudinal beam position distribution
        void setSparams( float s1, float s2 ) { s_ = params( s1, s2 ); }
        /// Set the lower and upper limits to the initial longitudinal beam position distribution
        void setSlimits( float s1, float s2 ) { s_ = parameters( s1, s2 ); }

        /// Set the parameters to the horizontal beam position distribution
        void setXparams( float x1, float x2 ) { x_ = params( x1, x2 ); }
        /// Set the lower and upper limits to the horizontal beam position distribution
        void setXlimits( float x1, float x2 ) { x_ = parameters( x1, x2 ); }

        /// Set the parameters to the vertical beam position distribution
        void setYparams( float y1, float y2 ) { y_ = params( y1, y2 ); }
        /// Set the lower and upper limits to the vertical beam position distribution
        void setYlimits( float y1, float y2 ) { y_ = parameters( y1, y2 ); }

        /// Set the parameters to the horizontal angular distribution
        void setTXparams( float tx1, float tx2 ) { tx_ = params( tx1, tx2 ); }
        /// Set the lower and upper limits to the horizontal angular distribution
        void setTXlimits( float tx1, float tx2 ) { tx_ = parameters( tx1, tx2 ); }

        /// Set the parameters to the vertical angular distribution
        void setTYparams( float ty1, float ty2 ) { ty_ = params( ty1, ty2 ); }
        /// Set the lower and upper limits to the vertical angular parameter distribution
        void setTYlimits( float ty1, float ty2 ) { ty_ = parameters( ty1, ty2 ); }

        //----- Single particle information

        /// Set the outgoing particles' mass (in GeV)
        void setParticleMass( float mass ) { mass_ = mass; }
        /// Set the outgoing particles' charge (in e)
        void setParticleCharge( float q ) { charge_ = q; }

      private:
        /// Translate lower and upper limits into parameters to give to the random generator
        params parameters( const float& lim1, const float& lim2 ) { return params( lim1, lim2 ); }

        params e_, s_;
        params x_, y_;
        params tx_, ty_;
        float mass_, charge_;
    };

    /// Beam of particles with flat s, x, y, Tx, Ty and energy distributions
    typedef ParticleGun<CLHEP::RandFlat> flatParticleGun;
    /// Beam of particles with gaussian s, x, y, Tx, Ty and energy distributions
    typedef ParticleGun<CLHEP::RandGauss> gaussianParticleGun;

    /// Specialization for Gaussian parameters
    template<> params gaussianParticleGun::parameters( const float& lim1, const float& lim2 );
  }
}

#endif
