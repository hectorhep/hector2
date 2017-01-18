#include "BeamProducer.h"

namespace Hector
{
  /*const float half_crossing_angle = Constants::crossing_angle/2., // urad (at IP)
              s_ini = 0.,
              mp = 0.938;
  // default kinematics parameters
  CLHEP::Hep2Vector pos_ini( -500., 0. );
  CLHEP::HepLorentzVector mom_ini( half_crossing_angle, 0., 0., mp );*/

  Particles
  BeamProducer::Xscanner( const size_t& num_part, float e_ini, float x_min, float x_max, float y, float s_ini )
  {
    Particles out;
    for ( size_t i=0; i<num_part; i++ ) {
      const float x = x_min + i*( x_max-x_min )/( num_part-1 );
      const CLHEP::Hep2Vector pos_ini( x, y );
      const CLHEP::HepLorentzVector mom_ini( 0., 0., 0., Constants::beam_energy );
      Particle::StateVector vec( mom_ini, pos_ini );
      out.push_back( Particle( vec, s_ini ) );
    }
    return out;
  }

  Particles
  BeamProducer::Yscanner( const size_t& num_part, float e_ini, float y_min, float y_max, float x, float s_ini )
  {
    Particles out;
    for ( size_t i=0; i<num_part; i++ ) {
      const float y = y_min + i*( y_max-y_min )/( num_part-1 );
      const CLHEP::Hep2Vector pos_ini( x, y );
      const CLHEP::HepLorentzVector mom_ini( 0., 0., 0., Constants::beam_energy );
      Particle::StateVector vec( mom_ini, pos_ini );
      out.push_back( Particle( vec, s_ini ) );
    }
    return out;
  }

  Particles
  BeamProducer::TXscanner( const size_t& num_part, float e_ini, float tx_min, float tx_max, float ty, float s_ini )
  {
    Particles out;
    for ( size_t i=0; i<num_part; i++ ) {
      const float tx = tx_min + i*( tx_max-tx_min )/( num_part-1 );
      Particle::StateVector vec;
      vec[Particle::StateVector::TX] = tx;
      vec[Particle::StateVector::TY] = ty;
      vec.setEnergy( e_ini );
      out.push_back( Particle( vec, s_ini ) );
    }
    return out;
  }

  Particles
  BeamProducer::TYscanner( const size_t& num_part, float e_ini, float ty_min, float ty_max, float tx, float s_ini )
  {
    Particles out;
    return out;
  }

  Particles
  BeamProducer::gaussianParticleGun( const size_t& num_part,
                                     float e_min, float e_max, float s_min, float s_max,
                                     float x_min, float x_max, float y_min, float y_max,
                                     float tx_min, float tx_max, float ty_min, float ty_max,
                                     float mass, float charge )
  {
    Particles out;

    for ( size_t i=0; i<num_part; i++ ) {
      Particle::StateVector vec;
      const float s = CLHEP::RandGauss::shoot( ( s_min+s_max )/2., ( s_max-s_min )/2. );
      vec[Particle::StateVector::X] = CLHEP::RandGauss::shoot( (x_min+x_max)/2., ( x_max-x_min )/2. );
      vec[Particle::StateVector::Y] = CLHEP::RandGauss::shoot( (y_min+y_max)/2., ( y_max-y_min )/2. );
      vec[Particle::StateVector::TX] = CLHEP::RandGauss::shoot( (tx_min+tx_max)/2., ( tx_max-tx_min )/2. );
      vec[Particle::StateVector::TY] = CLHEP::RandGauss::shoot( (ty_min+ty_max)/2., ( ty_max-ty_min )/2. );
      vec[Particle::StateVector::E] = CLHEP::RandGauss::shoot( (e_min+e_max)/2., ( e_max-e_min )/2. );
      //vec.setM( mass );

      Particle p( vec, s );
      p.setCharge( charge );
      std::cout << e_min << " -- " << e_max << ":" << p.firstStateVector().energy() << std::endl;
      //p.dump();
      out.push_back( p );
    }
    return out;
  }

  Particles
  BeamProducer::flatParticleGun( const size_t& num_part,
                                 float e_min, float e_max, float s_min, float s_max,
                                 float x_min, float x_max, float y_min, float y_max,
                                 float tx_min, float tx_max, float ty_min, float ty_max,
                                 float mass, float charge )
  {
    Particles out;

    for ( size_t i=0; i<num_part; i++ ) {
      Particle::StateVector vec;
      const float s = CLHEP::RandFlat::shoot( s_min, s_max );
      vec[Particle::StateVector::X] = CLHEP::RandFlat::shoot( x_min, x_max );
      vec[Particle::StateVector::Y] = CLHEP::RandFlat::shoot( y_min, y_max );
      vec[Particle::StateVector::TX] = CLHEP::RandFlat::shoot( tx_min, tx_max );
      vec[Particle::StateVector::TY] = CLHEP::RandFlat::shoot( ty_min, ty_max );
      vec[Particle::StateVector::E] = CLHEP::RandFlat::shoot( e_min, e_max );
      vec.setM( mass );

      Particle p( vec, s );
      p.setCharge( charge );
      out.push_back( p );
    }
    return out;
  }
}
