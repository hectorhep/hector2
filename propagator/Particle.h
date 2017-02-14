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
      class StateVector : private CLHEP::HepVector
      {
        private:
          /// Human-readable enumeration of the 6 state vector coordinates
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

          /// Human-readable printout of the state vector
          friend std::ostream& operator<<( std::ostream&, const StateVector& );

          /// Get the 6-vector associated to this state
          const CLHEP::HepVector& vector() const { return *this; }

          /// Set the particle energy (in GeV)
          void setEnergy( float energy ) { ( *this )[E] = energy; }
          /// Particle energy (in GeV)
          float energy() const { return ( *this )[E]; }
          /// Set the energy loss \f$ \xi \f$
          void setXi( float xi ) { setEnergy( Constants::beam_energy*( 1.-xi ) ); }
          /// Energy loss \f$ \xi \f$
          float xi() const { return 1.-energy()/Constants::beam_energy; }
          /// Particle kick
          float kick() const { return ( *this )[K]; }

          /// Fill the components of a state according to the particle position
          void setPosition( const CLHEP::Hep2Vector& pos );
          /// x-y position of a particle (in m)
          CLHEP::Hep2Vector position() const;
          /// Fill the components of a state according to the particle x'-y' angles (in rad)
          void setAngles( const CLHEP::Hep2Vector& angles );
          /// x'-y' angles of a particles (in rad)
          CLHEP::Hep2Vector angles() const;

          /// Fill the components of a state according to the particle kinematics
          void setMomentum( const CLHEP::HepLorentzVector& mom );
          /// Four-momentum of the particle (in GeV)
          CLHEP::HepLorentzVector momentum() const;
          /// Set the particle mass (in GeV)
          /// \warning Use with care! edits the energy as well as the mass to maintain a consistant kinematics.
          void setM( float mass );
          /// Particle mass (in GeV)
          float m() const { return m_; }
          /// Horizontal component of the polar angle
          float Tx() const { return ( *this )[TX]; }
          /// Vertical component of the polar angle
          float Ty() const { return ( *this )[TY]; }

        private:
          float m_;
      };
      /// Particle trajectory holder ; map of state vectors indexed to the s-position
      typedef std::map<float,StateVector> PositionsMap;

    public:
      Particle();
      /// Construct a particle according to its first state vector's 4-momentum
      Particle( const CLHEP::HepLorentzVector& mom ) : Particle( StateVector( mom ) ) {}
      /// Construct a particle according to its first state vector/s-position couple
      Particle( const StateVector&, float s0=0. );
      ~Particle();

    public:
      /// A s-position/state vector couple providing the full particle position
      class Position : private std::pair<float,StateVector>
      {
        public:
          /// Construct a couple between a longitudinal position and a state vector
          Position( const float& s, const StateVector& sv ) : std::pair<float,StateVector>( s, sv ) {}
          /// Longitudinal position
          const float& s() const { return this->first; }
          /// State vector components
          const StateVector stateVector() const { return this->second; }
          /// A pair of longitudinal position/state vector
          const std::pair<float,StateVector>& pair() const { return ( *this ); }
      };

      /// Build a Particle object from a mass and electric charge
      static Particle fromMassCharge( float mass, float charge );

      /// Clear all state vectors (but the initial one)
      void clear() { positions_.erase( ++begin(), end() ); }

      /// Set the electric charge (in e)
      void setCharge( int ch ) { charge_ = ch; }
      /// Electric charge (in e)
      int charge() const { return charge_; }

      /// Set the particle's PDG id
      void setPDGid( int pdgid ) { pdgId_ = pdgid; }
      /// Particle's PDG id
      int pdgId() const { return pdgId_; }

      /// Mass (in GeV/c2)
      float mass() const { return firstStateVector().m(); }

      /// Print all useful information about a particle
      void dump( std::ostream& os=std::cout ) const;

      /// First s position-state vector couple recorded
      Position firstPosition() { return Position( positions_.begin()->first, positions_.begin()->second ); }
      /// Last s position-state vector couple recorded
      Position lastPosition() { return Position( positions_.rbegin()->first, positions_.rbegin()->second ); }

      /// First position associated to the particle along s
      const float firstS() const { return positions_.begin()->first; }
      /// Last position associated to the particle along s
      const float lastS() const { return positions_.rbegin()->first; }

      /// First state vector associated to the particle
      StateVector& firstStateVector() { return positions_.begin()->second; }
      /// Last state vector associated to the particle
      StateVector& lastStateVector() { return positions_.rbegin()->second; }
      /// First state vector associated to the particle
      const StateVector& firstStateVector() const { return positions_.begin()->second; }
      /// Last state vector associated to the particle
      const StateVector& lastStateVector() const { return positions_.rbegin()->second; }

      /// Iterator to the first s-position/state vector couple of the particle's trajectory
      PositionsMap::iterator begin() { return positions_.begin(); }
      /// Iterator to the last s-position/state vector couple of the particle's trajectory
      PositionsMap::iterator end() { return positions_.end(); }
      /// Iterator to the first s-position/state vector couple of the particle's trajectory
      const PositionsMap::const_iterator begin() const { return positions_.begin(); }
      /// Iterator to the last s-position/state vector couple of the particle's trajectory
      const PositionsMap::const_iterator end() const { return positions_.end(); }

      /// Add a new s-position/state vector couple to the particle's trajectory
      void addPosition( const Position&, bool stopped=false );
      /// Add a new s-position/state vector couple to the particle's trajectory
      void addPosition( float s, const StateVector& sv, bool stopped=false ) { addPosition( Position( s, sv ), stopped ); }

      /// Let the particle emit a photon
      void emitGamma( float e_gamma, float q2, float phi_min=0., float phi_max=2*CLHEP::pi );

    private:
      int charge_;
      int pdgId_;
      bool stopped_;

      PositionsMap positions_;

  };

  /// Collection of particles composing the beam
  typedef std::vector<Particle> Particles;
}

#endif
