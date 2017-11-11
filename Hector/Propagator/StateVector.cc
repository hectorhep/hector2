#include "Hector/Propagator/StateVector.h"
#include "Hector/Core/Exception.h"

#include <CLHEP/Matrix/Matrix.h>
#include <CLHEP/Vector/LorentzVector.h>

namespace Hector
{
  StateVector::StateVector() :
    CLHEP::HepVector( 6, 0 ), m_( 0. )
  {
    ( *this )[K] = 1.;
    ( *this )[E] = Parameters::get()->beamEnergy();
  }

  StateVector::StateVector( const CLHEP::HepVector& vec, double mass ) :
    CLHEP::HepVector( vec ), m_( mass )
  {}

  StateVector::StateVector( const CLHEP::HepLorentzVector& mom, const CLHEP::Hep2Vector& pos ) :
    CLHEP::HepVector( 6, 1 ), m_( mom.m() )
  {
    setPosition( pos );
    setMomentum( mom );
    ( *this )[K] = 1.;
  }

  StateVector::StateVector( const CLHEP::Hep2Vector& pos, const CLHEP::Hep2Vector& ang, double energy, double kick ) :
    CLHEP::HepVector( 6, 1 ), m_( 0. )
  {
    setPosition( pos );
    setAngles( ang );
    setEnergy( energy );
    setKick( kick );
  }

  void
  StateVector::setPosition( double x, double y )
  {
    // store in m
    ( *this )[X] = x;
    ( *this )[Y] = y;
  }

  CLHEP::Hep2Vector
  StateVector::position() const
  {
    // return in m
    return CLHEP::Hep2Vector( ( *this )[X], ( *this )[Y] );
  }

  void
  StateVector::setAngles( double tx, double ty )
  {
    // store the tangent of the angles
    ( *this )[TX] = tan( tx );
    ( *this )[TY] = tan( ty );
  }

  CLHEP::Hep2Vector
  StateVector::angles() const
  {
    // return in rad
    return math::atan2( CLHEP::Hep2Vector( ( *this )[TX], ( *this )[TY] ) );
  }

  void
  StateVector::setMomentum( const CLHEP::HepLorentzVector& mom )
  {
    setAngles( atan2( mom.px(), mom.pz() ), atan2( mom.py(), mom.pz() ) );
    ( *this )[E] = mom.e();
    m_ = mom.m();
  }

  void
  StateVector::addMomentum( const CLHEP::HepLorentzVector& mom )
  {
    setAngles( angles()+math::atan2( CLHEP::Hep2Vector( atan2( mom.px(), mom.pz() ), atan2( mom.py(), mom.pz() ) ) ) );
    ( *this )[E] = mom.e();
    m_ = mom.m();
  }

  CLHEP::HepLorentzVector
  StateVector::momentum() const
  {
    const CLHEP::Hep2Vector tan_ang( math::tan2( angles() ) );
    const double pz = sqrt( ( energy()*energy()-m_*m_ )/( 1.+tan_ang.mag2() ) ),
                 px = pz*tan_ang.x(),
                 py = pz*tan_ang.y();
    return CLHEP::HepLorentzVector( px, py, pz, energy() );
  }

  void
  StateVector::setM( double mass )
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
