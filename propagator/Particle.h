#ifndef Hector_Propagator_Particle_h
#define Hector_Propagator_Particle_h

#include <CLHEP/Vector/TwoVector.h>
#include <CLHEP/Vector/LorentzVector.h>
#include <CLHEP/Units/SystemOfUnits.h> // pi
#include "StateVector.h"

#include <vector>

#include "core/Exception.h"

using std::cout;

namespace Hector
{
  /// Generic particle model inserted in a beam
  class Particle
  {
    public:
      /// A s-position/state vector couple providing the full particle position
      class Position : private std::pair<float,StateVector>
      {
        public:
          /// Construct a couple between a longitudinal position and a state vector
          Position( const float& s, const StateVector& sv ) : std::pair<float,StateVector>( s, sv ) {}
          /// Longitudinal position
          const float s() const { return this->first; }
          /// State vector components
          StateVector& stateVector() { return this->second; }
          /// State vector components
          const StateVector stateVector() const { return this->second; }
          /// A pair of longitudinal position/state vector
          const std::pair<float,StateVector>& pair() const { return ( *this ); }
      };
      /// Particle trajectory holder ; map of state vectors indexed to the s-position
      typedef std::vector<Position> PositionsMap;
      /// Sorting methods for the positions map construction (using the s position of each positions)
      struct PositionsSorter
      {
        /// Compare the references to two elements
        inline bool operator()( const Position& lhs, const Position& rhs ) const {
          return ( lhs.s()<rhs.s() );
        }
        /// Compare the pointers to two elements
        inline bool operator()( const Position* lhs, const Position* rhs ) const {
          return ( lhs and rhs and lhs->s()<rhs->s() );
        }
      };

    public:
      Particle();
      /// Construct a particle according to its first state vector's 4-momentum
      Particle( const CLHEP::HepLorentzVector& mom ) : Particle( StateVector( mom ) ) {}
      /// Construct a particle according to its first state vector/s-position couple
      Particle( const StateVector&, float s0=0. );
      ~Particle();

      /// Build a Particle object from a mass and electric charge
      static Particle fromMassCharge( float mass, float charge );

      /// Clear all state vectors (but the initial one)
      void clear() { positions_.erase( ++begin(), end() ); }
      /// Add a new s-position/state vector couple to the particle's trajectory
      void addPosition( const Position&, bool stopped=false );
      /// Add a new s-position/state vector couple to the particle's trajectory
      void addPosition( float s, const StateVector& sv, bool stopped=false ) { addPosition( Position( s, sv ), stopped ); }
      /// Sort all positions according to their s-coordinate
      void sortPositions();

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
      Position firstPosition() { return *( positions_.begin() ); }
      /// Last s position-state vector couple recorded
      Position lastPosition() { return *( positions_.rbegin() ); }

      /// First position associated to the particle along s
      const float firstS() const { return positions_.begin()->s(); }
      /// Last position associated to the particle along s
      const float lastS() const { return positions_.rbegin()->s(); }

      /// First state vector associated to the particle
      StateVector& firstStateVector() { return positions_.begin()->stateVector(); }
      /// Last state vector associated to the particle
      StateVector& lastStateVector() { return positions_.rbegin()->stateVector(); }
      /// First state vector associated to the particle
      const StateVector firstStateVector() const { return positions_.begin()->stateVector(); }
      /// Last state vector associated to the particle
      const StateVector lastStateVector() const { return positions_.rbegin()->stateVector(); }

      /// Iterator to the first s-position/state vector couple of the particle's trajectory
      PositionsMap::iterator begin() { return positions_.begin(); }
      /// Iterator to the last s-position/state vector couple of the particle's trajectory
      PositionsMap::iterator end() { return positions_.end(); }
      /// Iterator to the first s-position/state vector couple of the particle's trajectory
      const PositionsMap::const_iterator begin() const { return positions_.begin(); }
      /// Iterator to the last s-position/state vector couple of the particle's trajectory
      const PositionsMap::const_iterator end() const { return positions_.end(); }

      /// Compute the value of the state vector at a given s-position
      StateVector stateVectorAt( float s ) const;
      /// Get the particle's momentum at a given s-position
      const CLHEP::HepLorentzVector momentumAt( float s ) const { return stateVectorAt( s ).momentum(); }

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
