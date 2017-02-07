#include "io/MADXParser.h"
#include "propagator/Propagator.h"
#include "propagator/BeamProducer.h"

int main( int argc, char* argv[] )
{
  Hector::Parser::MADX parser( "data/twiss/twiss_coll0p4m_ir5b1_6p5tev.tfs", "IP5", +1, 250. );

  std::cout << "beamline matrix: " << parser.beamline()->matrix( 100., Hector::Constants::beam_particles_mass, +1 ) << std::endl;

  Hector::Propagator prop( parser.beamline() );
  //parser.beamline()->dump();
  parser.printInfo();

  const unsigned short num_part = 100;
  //Hector::BeamProducer::gaussianParticleGun gun( Hector::Constants::beam_energy/1.25, Hector::Constants::beam_energy );
  Hector::BeamProducer::TXscanner gun( num_part, Hector::Constants::beam_energy, 0., 1. );
  for ( unsigned int i=0; i<num_part; i++ ) {
    Hector::Particle p = gun.shoot();
    p.setCharge( +1 );
    try {
      prop.propagate( p, 215.5 );
    } catch ( Hector::Exception& e ) { e.dump(); }
    std::cout << p.lastPosition().stateVector().position() << std::endl;
  }

  return 0;
}
