#include "io/Pythia8Generator.h"
#include "io/MADXParser.h"
#include "propagator/Propagator.h"

#include "test/Canvas.h"
#include "TH2.h"
#include "TH1.h"

int main( int argc, char* argv[] )
{
  if ( argc<2 ) {
    std::cerr << "Usage: " << argv[0] << " <mad-x twiss file>" << std::endl;
    return -1;
  }
  Hector::Parser::MADX madx( argv[1], "IP5", 1, 250. );
  //madx.beamline()->offsetElementsAfter( 120., CLHEP::Hep2Vector( -0.097, 0. ) );
  Hector::Propagator prop( madx.beamline() );
  std::cout << "---> " << madx.romanPots().size() << std::endl;
  const Hector::Elements rps = madx.romanPots( Hector::Parser::MADX::horizontalPots );
  for ( Hector::Elements::const_iterator rp=rps.begin(); rp!=rps.end(); rp++ ) {
    std::cout << "--------> " << ( *rp )->name() << " at " << ( *rp )->s() << std::endl;
  }
  const float s_pos = 220.;

  // number of events to generate
  const unsigned short num_events = 10000;

  Hector::Pythia8Generator gen;

  TH2D h_hitmap( "hitmap", "x (m)\\y (m)", 200, -0.1, 0.1, 200, -0.1, 0.1 );
  TH1D h_xidiff( "xi", "#xi\\Events\\?.3f", 200, -0.5, 0.5 );

  for ( unsigned short i=0; i<num_events; i++ ) {
    Hector::Particles part = gen.generate();
    try {
      prop.propagate( part, s_pos );
      for ( Hector::Particles::iterator it=part.begin(); it!=part.end(); it++ ) {
        Hector::Particle p = *( it );
        h_xidiff.Fill( p.firstStateVector().xi() );
        //std::cout << "---> " << p.firstStateVector() << std::endl;
        //std::cout << "-------> " << p.firstStateVector().momentum() << std::endl;
        const CLHEP::Hep2Vector pos = p.stateVectorAt( s_pos ).position();
        h_hitmap.Fill( pos.x(), pos.y() );
      }
    } catch ( Hector::Exception& e ) {}
  }

  {
    Hector::Canvas c( "xi_single_diffr", "" );
    h_xidiff.Draw();
    c.Prettify( &h_xidiff );
    c.SetLogy();
    c.Save( "pdf" );
  }

  {
    Hector::Canvas c( "hitmap_single_diffr", "" );
    h_hitmap.Draw( "colz" );
    c.Prettify( &h_hitmap );
    c.Save( "pdf" );
  }

  return 0;
}
