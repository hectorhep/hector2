#include "io/Pythia8Generator.h"
#include "io/MADXParser.h"
#include "propagator/Propagator.h"

#include "test/Canvas.h"
#include "TH2.h"

int main( int argc, char* argv[] )
{
  if ( argc<2 ) {
    std::cerr << "Usage: " << argv[0] << " <mad-x twiss file>" << std::endl;
    return -1;
  }
  Hector::Parser::MADX madx( argv[1], "IP5", +1, 250. );
  Hector::Propagator prop( madx.beamline() );
  const float s_pos = 220.;

  // number of events to generate
  const unsigned short num_events = 10000;

  Hector::Pythia8Generator gen;

  TH2D h_hitmap( "hitmap", "x (m)\\y (m)", 200, -0.1, 0.1, 200, -0.1, 0.1 );

  for ( unsigned short i=0; i<num_events; i++ ) {
    Hector::Particles part = gen.generate();
    for ( Hector::Particles::iterator it=part.begin(); it!=part.end(); it++ ) {
      Hector::Particle part = *( it );
      prop.propagate( part, s_pos );
      try {
        const CLHEP::Hep2Vector pos = part.stateVectorAt( s_pos ).position();
        h_hitmap.Fill( pos.x(), pos.y() );
      } catch ( Hector::Exception& e ) { e.dump(); }
    }
  }

  {
    Hector::Canvas c( "hitmap_single_diffr", "" );
    h_hitmap.Draw( "colz" );
    c.Prettify( &h_hitmap );
    c.Save( "pdf" );
  }

  return 0;
}
