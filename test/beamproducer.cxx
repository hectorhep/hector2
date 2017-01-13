#include "beamline/Beamline.h"
#include "io/MADXParser.h"
#include "propagator/Propagator.h"

#include "utils.h"
#include "Canvas.h"

#include <CLHEP/Random/RandGauss.h>
#include <CLHEP/Units/SystemOfUnits.h>

#include "TH2.h"
#include "TH1.h"

int
main( int argc, char* argv[] )
{
  if ( argc<3 ) {
    std::cout << "Usage: " << argv[0] << " [MAD-X output file] [maximum] <IP name>" << std::endl;
    return -1;
  }
  const float s_max = atof( argv[2] );
  const char* ip = ( argc>3 ) ? argv[3] : "IP5";

  Hector::Parser::MADX parser( argv[1], ip, +1, s_max );
  parser.printInfo();

  // look at both the beamlines

  Hector::Propagator prop( parser.beamline() );

  Hector::BeamProducer::Particles part = Hector::BeamProducer::TXscanner( 100, Hector::Constants::beam_energy, 0., 10., s_max );
  //Hector::BeamProducer::Particles part = Hector::BeamProducer::particleGun( 2, 6000., 6500. );
  for ( Hector::BeamProducer::Particles::const_iterator p=part.begin(); p!=part.end(); p++ ) {
    //std::cout << "--> " << p->firstStateVector().energy() << " ---> " << p->firstStateVector().m() << " | " << p->firstStateVector().momentum() << std::endl;
    p->dump();
  }

  return 0;
}
