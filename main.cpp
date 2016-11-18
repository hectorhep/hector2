#include "io/MADXParser.h"
#include "propagator/Propagator.h"

int main( int argc, char* argv[] )
{
  //Parser::MADX parser( "data/twiss/twiss_coll0p4m_ir5b1_6p5tev.tfs", +1, 0.3 );
  Parser::MADX parser( "data/twiss/twiss_coll0p4m_ir5b1_6p5tev.tfs", +1, 250. );

  //parser.beamline()->dump();
  std::cout << "beamline matrix: " << parser.beamline()->matrix( 100., 0.938, +1 ) << std::endl;

  Propagator prop( parser.beamline() );

  for ( unsigned int i=0; i<1000; i++ ) {
    CLHEP::HepLorentzVector mom0( 0., 0.1*i, 0., 0.938 );
    Particle p( mom0 );
    p.setCharge( +1 );

    prop.propagate( p, 100. );
    //p.dump();
  }

  return 0;
}
