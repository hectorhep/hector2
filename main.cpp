#include "io/MADXParser.h"
#include "propagator/Propagator.h"

int main( int argc, char* argv[] )
{
  //Parser::MADX parser( "data/twiss/twiss_coll0p4m_ir5b1_6p5tev.tfs", "IP5", +1, 0.3 );
  Parser::MADX parser( "data/twiss/twiss_coll0p4m_ir5b1_6p5tev.tfs", "IP5", +1, 250. );

  //parser.beamline()->dump();
  std::cout << "beamline matrix: " << parser.beamline()->matrix( 100., 0.938, +1 ) << std::endl;

  Propagator prop( parser.beamline() );

  CLHEP::HepLorentzVector mom0( 0., 0., 0., 0.938 );
  for ( unsigned int i=0; i<100; i++ ) {
    //mom0.setE( 100.+i*10. );
    const float beta = 0.999 + i*1.e-5;
    mom0.boostZ( beta );
std::cout << beta << " -> " << mom0 << std::endl;
    Particle p( mom0 );
    p.setCharge( +1 );

/*    prop.propagate( p, 100. );
    p.dump();*/
  }

  return 0;
}
