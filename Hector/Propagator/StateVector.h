#ifndef Hector_Propagator_StateVector_h
#define Hector_Propagator_StateVector_h

#include <CLHEP/Matrix/Vector.h>
#include <CLHEP/Vector/TwoVector.h>
#include <CLHEP/Vector/LorentzVector.h>

#include <map>

#include "Core/Parameters.h"

using std::cout;

namespace Hector
{
  /// Six-dimensional state vector associated to a particle at a given s
  class StateVector : private CLHEP::HepVector
  {
    private:
      /// Human-readable enumeration of the 6 state vector coordinates
      enum Components {
        X  = 0, TX = 1,
        Y  = 2, TY = 3,
        E  = 4, K  = 5
      };
    public:
      /// Build a blank state
      StateVector() : CLHEP::HepVector( 6, 0 ), m_( 0. ) {
        ( *this )[K] = 1.;
        ( *this )[E] = Parameters::get()->beamEnergy();
      }
      //StateVector( const StateVector& sv ) : CLHEP::HepVector( sv.vector() ), m_( sv.m_ ) {}
      /// Build a state using a 6-component vector and a particle mass
      /// \param[in] vec A 6-component vector
      /// \param[in] mass Particle mass (GeV)
      StateVector( const CLHEP::HepVector& vec, double mass );
      /// Build a state using a particle kinematics and position
      /// \param[in] mom Four-momentum of the particle (GeV)
      /// \param[in] pos x-y position of the particle (m)
      StateVector( const CLHEP::HepLorentzVector& mom, const CLHEP::Hep2Vector& pos=CLHEP::Hep2Vector() );
      /// Build a state vector using the particle's position and its angle
      StateVector( const CLHEP::Hep2Vector& pos, const CLHEP::Hep2Vector& ang, double energy=Parameters::get()->beamEnergy(), double kick=1. );
      ~StateVector() {}

      /// Human-readable printout of the state vector
      friend std::ostream& operator<<( std::ostream&, const StateVector& );

      /// Get the 6-vector associated to this state
      const CLHEP::HepVector& vector() const { return *this; }

      /// Set the particle energy (in GeV)
      void setEnergy( double energy ) { ( *this )[E] = energy; }
      /// Particle energy (in GeV)
      double energy() const { return ( *this )[E]; }
      /// Set the energy loss \f$ \xi \f$
      void setXi( double xi ) { setEnergy( Parameters::get()->beamEnergy()*( 1.-xi ) ); }
      /// Energy loss \f$ \xi \f$
      double xi() const { return 1.-energy()/Parameters::get()->beamEnergy(); }
      /// Set the particle kick
      void setKick( double kick ) { ( *this )[K] = kick; }
      /// Particle kick
      double kick() const { return ( *this )[K]; }

      /// Fill the components of a state according to the particle position
      void setPosition( double x, double y );
      /// Fill the components of a state according to the particle position
      void setPosition( const CLHEP::Hep2Vector& pos ) { setPosition( pos.x(), pos.y() ); }
      /// x-y position of a particle (in m)
      CLHEP::Hep2Vector position() const;
      /// Fill the components of a state according to the particle x'-y' angles (in rad)
      void setAngles( double tx, double ty );
      /// Fill the components of a state according to the particle x'-y' angles (in rad)
      void setAngles( const CLHEP::Hep2Vector& angles ) { setAngles( angles.x(), angles.y() ); }
      /// x'-y' polar angles of a particles (in rad)
      CLHEP::Hep2Vector angles() const;

      /// Fill the components of a state according to the particle kinematics
      void setMomentum( const CLHEP::HepLorentzVector& mom );
      void addMomentum( const CLHEP::HepLorentzVector& mom );
      /// Four-momentum of the particle (in GeV)
      CLHEP::HepLorentzVector momentum() const;
      /// Set the particle mass (in GeV)
      /// \warning Use with care! edits the energy as well as the mass to maintain a consistant kinematics.
      void setM( double mass );
      /// Particle mass (in GeV)
      double m() const { return m_; }

    private:
      double m_;
  };
}

#endif
