#include "Hector/Core/Exception.h"

#include "Hector/IO/MADXHandler.h"
#include "Hector/Beamline/Beamline.h"

#include "Hector/Propagator/Propagator.h"
#include "Hector/Utils/BeamProducer.h"
#include "Hector/Utils/ArgsParser.h"

#include <CLHEP/Random/RandGauss.h>

using namespace std;

int main( int argc, char* argv[] )
{
  string twiss_file, ip;
  double max_s;
  unsigned int num_part = 100;
  Hector::ArgsParser( argc, argv, {}, {
    { "--twiss-file", "beamline Twiss file", "data/twiss/twiss_coll0p4m_ir5b1_6p5tev.tfs", &twiss_file },
    { "--interaction-point", "name of the interaction point", "IP5", &ip },
    { "--max-s", "maximum arc length s to parse", 250., &max_s },
    { "--num-part", "number of particles to shoot", 10, &num_part },
  } );
  Hector::IO::MADX parser( twiss_file.c_str(), ip.c_str(), +1, max_s );
  parser.printInfo();
  parser.beamline()->dump();

  cout << "beamline matrix at s = " << max_s << " m: " << parser.beamline()->matrix( 100., Hector::Parameters::get()->beamParticlesMass(), +1 ) << endl;

  Hector::Propagator prop( parser.beamline() );
  //parser.beamline()->dump();

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
