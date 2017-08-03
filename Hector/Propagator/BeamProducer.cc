#include "BeamProducer.h"

namespace Hector
{
  /*const float half_crossing_angle = Constants::crossing_angle/2., // urad (at IP)
              s_ini = 0.,
              mp = 0.938;
  // default kinematics parameters
  CLHEP::Hep2Vector pos_ini( -500., 0. );
  CLHEP::HepLorentzVector mom_ini( half_crossing_angle, 0., 0., mp );*/

  unsigned short
  BeamProducer::LinearScanner::LinearScanner::next()
  {
    if ( num_gen_>=num_part_ ) {
      throw Exception( __PRETTY_FUNCTION__, "Too much particles already generated!", JustWarning );
    }
    return num_gen_++;
  }

  Particle
  BeamProducer::Xscanner::shoot()
  {
    const float x = p1_.first + LinearScanner::next()*( p1_.second-p1_.first )/( num_part_-1 ),
                y = p2_.first;

    const CLHEP::Hep2Vector pos_ini( x, y );
    const CLHEP::HepLorentzVector mom_ini( 0., 0., 0., Parameters::beamEnergy() );

    return Particle( StateVector( mom_ini, pos_ini ), s_.first );
  }

  Particle
  BeamProducer::Yscanner::shoot()
  {
    const float x = p2_.first,
                y = p1_.first + LinearScanner::next()*( p1_.second-p1_.first )/( num_part_-1 );

    const CLHEP::Hep2Vector pos_ini( x, y );
    const CLHEP::HepLorentzVector mom_ini( 0., 0., 0., Parameters::beamEnergy() );

    return Particle( StateVector( mom_ini, pos_ini ), s_.first );
  }

  Particle
  BeamProducer::TXscanner::shoot()
  {
    const float tx = p1_.first + LinearScanner::next()*( p1_.second-p1_.first )/( num_part_-1 ),
                ty = p2_.first;

    return Particle( StateVector( CLHEP::Hep2Vector(), CLHEP::Hep2Vector( tx, ty ), e_.first ), s_.first );
  }

  Particle
  BeamProducer::TYscanner::shoot()
  {
    const float tx = p2_.first,
                ty = p1_.first + LinearScanner::next()*( p1_.second-p1_.first )/( num_part_-1 );

    return Particle( StateVector( CLHEP::Hep2Vector(), CLHEP::Hep2Vector( tx, ty ), e_.first ), s_.first );
  }

  Particle
  BeamProducer::Xiscanner::shoot()
  {
    const float energy = e_.first + LinearScanner::next()*( e_.second-e_.first )/( num_part_-1 ),
                mom = sqrt( energy*energy-pow( Parameters::beamParticlesMass(), 2 ) );
    return Particle( StateVector( CLHEP::HepLorentzVector( 0., 0., mom, energy ), CLHEP::Hep2Vector( p1_.first, p2_.first ) ), s_.first );
  }

  template<>
  BeamProducer::params
  BeamProducer::ParticleGun<CLHEP::RandGauss>::parameters( const float& lim1, const float& lim2 )
  {
    return BeamProducer::params( ( lim1+lim2 )/2., ( lim2-lim1 )/2. );
  }
}
