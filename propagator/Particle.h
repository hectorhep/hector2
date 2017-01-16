#ifndef core_Particle_h
#define core_Particle_h

#include <CLHEP/Matrix/Vector.h>
#include <CLHEP/Matrix/Matrix.h>
#include <CLHEP/Vector/TwoVector.h>
#include <CLHEP/Vector/LorentzVector.h>
#include <CLHEP/Units/SystemOfUnits.h> // pi
#include <CLHEP/Random/RandFlat.h>

#include <map>

#include "core/Exception.h"
#include "core/Constants.h"

using std::cout;

namespace Hector
{
  /// Generic particle model inserted in a beam
  class Particle
  {
    public:
      /// Six-dimensional state vector associated to a particle at a given s
      class StateVector : public CLHEP::HepVector
      {
        public:
          enum Components { X = 0, TX, Y, TY, E, K };
        public:
          StateVector() : CLHEP::HepVector( 6, 0 ), m_( 0. ) {
            ( *this )[K] = 1.;
            ( *this )[E] = Constants::beam_energy;
          }
          /// Build a state using a 6-component vector and a particle mass
          /// \param[in] vec A 6-component vector
          /// \param[in] mass Particle mass (GeV)
          StateVector( const CLHEP::HepVector& vec, float mass ) : CLHEP::HepVector( vec ), m_( mass ) {}
          /// Build a state using a particle kinematics and position
          /// \param[in] mom Four-momentum of the particle (GeV)
          /// \param[in] pos x-y position of the particle (m)
          StateVector( const CLHEP::HepLorentzVector& mom, const CLHEP::Hep2Vector& pos=CLHEP::Hep2Vector() ) :
            CLHEP::HepVector( 6, 1 ), m_( mom.m() ) {
            setPosition( pos );
            setMomentum( mom );
            ( *this )[K] = 1.;
          }
          ~StateVector() {}

          void setEnergy( float energy ) { ( *this )[E] = energy; }
          /// Particle energy (GeV)
          float energy() const { return ( *this )[E]; }
          float kick() const { return ( *this )[K]; }

          /// Fill the components of a state according to the particle position
          void setPosition( const CLHEP::Hep2Vector& pos );
          /// x-y position of a particle (m)
          CLHEP::Hep2Vector position() const;

          /// Fill the components of a state according to the particle kinematics
          void setMomentum( const CLHEP::HepLorentzVector& mom );
          /// Four-momentum of the particle (GeV)
          CLHEP::HepLorentzVector momentum() const;
          /// Set the particle mass (GeV)
          /// \warning Use with care! edits the energy as well as the mass to maintain a consistant kinematics.
          void setM( float mass );
          /// Particle mass (GeV)
          float m() const { return m_; }
          /// Horizontal component of the polar angle
          float Tx() const { return ( *this )[TX]; }
          /// Vertical component of the polar angle
          float Ty() const { return ( *this )[TY]; }

        private:
          float m_;
      };
      typedef std::map<float,StateVector> PositionsMap;

    public:
      Particle();
      Particle( const CLHEP::HepLorentzVector& mom ) : Particle( StateVector( mom ) ) {;}
      Particle( const StateVector&, float s0=0. );
      ~Particle();

      static Particle fromMassCharge( float mass, float charge );

      void clear() { positions_.erase( ++begin(), end() ); }

      void setCharge( int ch ) { charge_ = ch; }
      int charge() const { return charge_; }

      float mass() const { return firstStateVector().m(); }

      void dump( std::ostream& os=std::cout ) const;

      const float firstPosition() const { return positions_.begin()->first; }
      const float lastPosition() const { return positions_.rbegin()->first; }

      StateVector& firstStateVector() { return positions_.begin()->second; }
      StateVector& lastStateVector() { return positions_.rbegin()->second; }
      const StateVector& firstStateVector() const { return positions_.begin()->second; }
      const StateVector& lastStateVector() const { return positions_.rbegin()->second; }

      PositionsMap::iterator begin() { return positions_.begin(); }
      PositionsMap::iterator end() { return positions_.end(); }
      const PositionsMap::const_iterator begin() const { return positions_.begin(); }
      const PositionsMap::const_iterator end() const { return positions_.end(); }

      void addPosition( float, const StateVector&, bool stopped=false );

      void emitGamma( float e_gamma, float q2, float phi_min=0., float phi_max=2*CLHEP::pi );

    private:
      int charge_;
      bool stopped_;

      PositionsMap positions_;

  };

  /// Collection of particles composing the beam
  typedef std::vector<Particle> Particles;
}

#endif
