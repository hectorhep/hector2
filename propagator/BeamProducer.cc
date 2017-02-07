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
    const CLHEP::HepLorentzVector mom_ini( 0., 0., 0., Constants::beam_energy );

    return Particle( Particle::StateVector( mom_ini, pos_ini ), s_.first );
  }

  Particle
  BeamProducer::Yscanner::shoot()
  {
    const float x = p2_.first,
                y = p1_.first + LinearScanner::next()*( p1_.second-p1_.first )/( num_part_-1 );

    const CLHEP::Hep2Vector pos_ini( x, y );
    const CLHEP::HepLorentzVector mom_ini( 0., 0., 0., Constants::beam_energy );

    return Particle( Particle::StateVector( mom_ini, pos_ini ), s_.first );
  }

  Particle
  BeamProducer::TXscanner::shoot()
  {
    const float tx = p1_.first + LinearScanner::next()*( p1_.second-p1_.first )/( num_part_-1 ),
                ty = p2_.first;
    Particle::StateVector vec;
    vec.setAngles( CLHEP::Hep2Vector( tx, ty ) );
    vec.setEnergy( e_ini_ );

    return Particle( vec, s_.first );
  }

  Particle
  BeamProducer::TYscanner::shoot()
  {
    const float tx = p2_.first,
                ty = p1_.first + LinearScanner::next()*( p1_.second-p1_.first )/( num_part_-1 );
    Particle::StateVector vec;
    vec.setAngles( CLHEP::Hep2Vector( tx, ty ) );
    vec.setEnergy( e_ini_ );

    return Particle( vec, s_.first );
  }
}
