#include "Hector/IO/MADXHandler.h"
#include "Hector/Propagator/Propagator.h"
#include "Hector/Propagator/BeamProducer.h"

int main( int argc, char* argv[] )
{
  const char* twiss_file = ( argc>1 ) ? argv[1] : "data/twiss/twiss_coll0p4m_ir5b1_6p5tev.tfs";
  Hector::Parser::MADX parser( twiss_file, "IP5", +1, 250. );
  parser.printInfo();
  parser.beamline()->dump();

  std::cout << "beamline matrix at s = 250 m: " << parser.beamline()->matrix( 100., Hector::Parameters::get()->beamParticlesMass(), +1 ) << std::endl;

  Hector::Propagator prop( parser.beamline() );
  //parser.beamline()->dump();

  const unsigned short num_part = 100;
  Hector::BeamProducer::gaussianParticleGun gun( Hector::Parameters::get()->beamEnergy()/1.25, Hector::Parameters::get()->beamEnergy() );
  //Hector::BeamProducer::TXscanner gun( num_part, Hector::Parameters::get()->beamEnergy(), 0., 1. );
  for ( unsigned int i=0; i<num_part; i++ ) {
    Hector::Particle p = gun.shoot();
    p.setCharge( +1 );
    try {
      prop.propagate( p, 203.826 );
    } catch ( Hector::Exception& e ) { e.dump(); }
  }

  return 0;
}
