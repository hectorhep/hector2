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
  parser.printInfo();

  // look at both the beamlines

  Hector::Propagator prop( parser.beamline() );

  TH2D hitmap( "hitmap", "x (m)\\y (m)", 200, -20e-3, 20e-3, 200, -20e-3, 20e-3 );

  const unsigned int num_particles = 100;
  const float beam_lateral_width_ip = 16.63e-6, // in meters
              beam_angular_divergence_ip = 30.23e-6, // in radians
              particles_energy = 6000.; // in GeV
  Hector::BeamProducer::gaussianParticleGun gun;
  gun.setElimits( particles_energy );
  gun.setXparams( 0., beam_lateral_width_ip );
  gun.setYparams( 0., beam_lateral_width_ip );
  gun.setTXparams( Hector::Constants::crossing_angle, beam_angular_divergence_ip );
  gun.setTYparams( 0., beam_angular_divergence_ip );
  //Hector::BeamProducer::Xscanner gun( num_particles, Hector::Constants::beam_energy, 0., 0.1 );

  unsigned short num_stopped = 0;
  for ( size_t i=0; i!=num_particles; i++ ) {

    { // propagation through the beamline
      Hector::Particle p = gun.shoot();
      try {
        prop.propagate( p, s_pos );
        if ( prop.stopped( p, s_pos ) ) { std::cout << "prout" << std::endl; num_stopped++; continue; }
        const CLHEP::Hep2Vector pos( p.stateVectorAt( s_pos ).position() );
        std::cout << s_pos << " -> " << pos << std::endl;
        hitmap.Fill( pos.x(), pos.y() );
      } catch ( Hector::Exception& e ) { /*e.dump();*/ continue; }
    }
  }
  PrintInfo( Form( "%.1f%% of particles (%d/%d) stopped before s = %.2f", 100. * num_stopped/num_particles, num_stopped, num_particles, s_pos ) );

  {
    Hector::Canvas c( "hitmap", Form( "Hector v2 simulation, s = %.2f m", s_pos ) );
    hitmap.Draw( "colz" );

    const std::string file( argv[1] );
    Hector::Canvas::PaveText( 0.0, 0., 0.0015, 0.0015, Form( "#scale[0.3]{Beamline: %s}", file.substr( file.find_last_of( "/\\" )+1 ).c_str() ) ).Draw();

    c.Prettify( &hitmap );

    c.Save( "pdf" );
  }

  return 0;
}
