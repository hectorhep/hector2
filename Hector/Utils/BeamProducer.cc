#include "Hector/Utils/BeamProducer.h"
#include "Hector/Core/Exception.h"

#include <CLHEP/Random/RandFlat.h>
#include <CLHEP/Random/RandGauss.h>

namespace Hector
{
  unsigned short
  BeamProducer::LinearScanner::LinearScanner::next()
  {
    if ( num_gen_ >= num_part_ )
      throw Exception( __PRETTY_FUNCTION__, "Too much particles already generated!", JustWarning );

    return num_gen_++;
  }

  Particle
  BeamProducer::Xscanner::shoot()
  {
    const float x = p1_.first + LinearScanner::next()*( p1_.second-p1_.first )/( num_part_-1 ),
                y = p2_.first;

    const TwoVector pos_ini( x, y );
    const LorentzVector mom_ini( 0., 0., 0., Parameters::get()->beamEnergy() );

    return Particle( StateVector( mom_ini, pos_ini ), s_.first );
  }

  Particle
  BeamProducer::Yscanner::shoot()
  {
    const float x = p2_.first,
                y = p1_.first + LinearScanner::next()*( p1_.second-p1_.first )/( num_part_-1 );

    const TwoVector pos_ini( x, y );
    const LorentzVector mom_ini( 0., 0., 0., Parameters::get()->beamEnergy() );

    return Particle( StateVector( mom_ini, pos_ini ), s_.first );
  }

  Particle
  BeamProducer::TXscanner::shoot()
  {
    const float tx = p1_.first + LinearScanner::next()*( p1_.second-p1_.first )/( num_part_-1 ),
                ty = p2_.first;

    return Particle( StateVector( TwoVector(), TwoVector( tx, ty ), e_.first ), s_.first );
  }

  Particle
  BeamProducer::TYscanner::shoot()
  {
    const float tx = p2_.first,
                ty = p1_.first + LinearScanner::next()*( p1_.second-p1_.first )/( num_part_-1 );

    return Particle( StateVector( TwoVector(), TwoVector( tx, ty ), e_.first ), s_.first );
  }

  Particle
  BeamProducer::Xiscanner::shoot()
  {
    const float energy = e_.first + LinearScanner::next()*( e_.second-e_.first )/( num_part_-1 ),
                mom = sqrt( energy*energy-pow( Parameters::get()->beamParticlesMass(), 2 ) );
    return Particle( StateVector( LorentzVector( 0., 0., mom, energy ), TwoVector( p1_.first, p2_.first ) ), s_.first );
  }

  template<>
  BeamProducer::params_t
  BeamProducer::ParticleGun<CLHEP::RandGauss>::parameters( float lim1, float lim2 )
  {
    return BeamProducer::params_t( 0.5 * ( lim1+lim2 ), 0.5 * ( lim2-lim1 ) );
  }
}
