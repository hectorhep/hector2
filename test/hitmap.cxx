#include "beamline/Beamline.h"
#include "io/MADXParser.h"
#include "propagator/Propagator.h"
#include "propagator/BeamProducer.h"

#include "utils.h"
#include "Canvas.h"

#include <CLHEP/Units/SystemOfUnits.h>

#include "TH2.h"
#include "TH1.h"

int
main( int argc, char* argv[] )
{
  if ( argc<3 ) {
    std::cout << "Usage: " << argv[0] << " [MAD-X output file] [s position] <IP name>" << std::endl;
    return -1;
  }
  const float s_pos = atof( argv[2] );
  const char* ip = ( argc>3 ) ? argv[3] : "IP5";

  Hector::Parser::MADX parser( argv[1], ip, +1, s_pos );

  // look at both the beamlines

  Hector::Propagator prop( parser.beamline() );

  TH2D hitmap( "hitmap", "x (m)\\y (m)", 200, -2e-2, 2e-2, 200, -2e-2, 2e-2 );

  const unsigned int num_particles = 10000;
  Hector::BeamProducer::gaussianParticleGun gun( Hector::Constants::beam_energy/1.2, Hector::Constants::beam_energy );
  //Hector::BeamProducer::Xscanner gun( num_particles, Hector::Constants::beam_energy, 0., 0.1 );
  for ( size_t i=0; i!=num_particles; i++ ) {

    { // propagation through the beamline
      Hector::Particle p = gun.shoot();
      std::cout << p.firstPosition().stateVector().momentum() << std::endl;
      try { prop.propagate( p, s_pos ); } catch ( Hector::Exception& e ) { /*e.dump();*/ continue; }

      const Hector::Particle::Position last_pos = p.lastPosition();
      //std::cout << last_pos.stateVector().position() << std::endl;
      hitmap.Fill( last_pos.stateVector().position().x(), last_pos.stateVector().position().y() );
    }
    //p.dump();
  }

  {
    Hector::Canvas c( "hitmap", "" );
    hitmap.Draw( "colz" );
    c.Prettify( &hitmap );
    c.Save( "pdf" );
  }

  return 0;
}
