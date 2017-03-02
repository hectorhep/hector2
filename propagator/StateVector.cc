#include "StateVector.h"

namespace Hector
{
  StateVector::StateVector( const CLHEP::HepVector& vec, float mass ) :
    CLHEP::HepVector( vec ), m_( mass )
  {}

  StateVector::StateVector( const CLHEP::HepLorentzVector& mom, const CLHEP::Hep2Vector& pos ) :
    CLHEP::HepVector( 6, 1 ), m_( mom.m() )
  {
    setPosition( pos );
    setMomentum( mom );
    ( *this )[K] = 1.;
  }

  StateVector::StateVector( const CLHEP::Hep2Vector& pos, const CLHEP::Hep2Vector& ang, float energy ) :
    CLHEP::HepVector( 6, 1 ), m_( 0. )
  {
    setPosition( pos );
    setAngles( ang );
    setEnergy( energy );
  }

  void
  StateVector::setPosition( const float& x, const float& y )
  {
    // store in um
    ( *this )[X] = x * 1.e3;
    ( *this )[Y] = y * 1.e3;
  }

  CLHEP::Hep2Vector
  StateVector::position() const
  {
    // return in m
    return CLHEP::Hep2Vector( ( *this )[X] * 1.e-3, ( *this )[Y] * 1.e-3 );
  }

  void
  StateVector::setAngles( const float& tx, const float& ty )
  {
    // store in urad
    ( *this )[TX] = tx * 1.e6;
    ( *this )[TY] = ty * 1.e6;
  }

  CLHEP::Hep2Vector
  StateVector::angles() const
  {
    // return in rad
    return CLHEP::Hep2Vector( ( *this )[TX] * 1.e-6, ( *this )[TY] * 1.e-6 );
  }

  void
  StateVector::setMomentum( const CLHEP::HepLorentzVector& mom )
  {
    setAngles( atan2( mom.px(), mom.pz() ), atan2( mom.py(), mom.pz() ) );
    ( *this )[E] = mom.e();
  }

  CLHEP::HepLorentzVector
  StateVector::momentum() const
  {
    const CLHEP::Hep2Vector ang( angles() );
    const float pz = sqrt( ( energy()*energy()-m_*m_ )/( 1.+pow( tan( ang.x() ), 2 )+pow( tan( ang.y() ), 2 ) ) ),
                px = pz*tan( ang.x() ),
                py = pz*tan( ang.y() );
    return CLHEP::HepLorentzVector( px, py, pz, energy() );
  }

  void
  StateVector::setM( float mass )
  {
    if ( mass!=momentum().m() ) {
      m_ = mass;
      if ( momentum().mag2()!=0. )
        ( *this )[E] = sqrt( momentum().mag2()+m_*m_ ); // match the energy accordingly
      else {
        const int sign = 1; //FIXME
        setMomentum( CLHEP::HepLorentzVector( 0., 0., sign*sqrt( energy()*energy()-m_*m_ ), energy() ) ); // longitudinal momentum only
      }
      return;
    }
    m_ = mass;
  }

  /// Human-readable printout of a state vector
  std::ostream&
  operator<<( std::ostream& os, const StateVector& vec )
  {
    return os << Form( "{ x = % 6.4e m, x' = % 6.3e rad, y = % 6.4e m, y' = % 6.3e rad, k = % 3.1f, E = %6.3e GeV }",
                       vec.position().x(), vec.angles().x(), vec.position().y(), vec.angles().y(), vec.kick(), vec.energy() );
  }
}
