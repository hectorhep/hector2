#include "Hector/Beamline/Beamline.h"
#include "Hector/IO/MADXHandler.h"
#include "Hector/Propagator/Propagator.h"
#include "Hector/Propagator/BeamProducer.h"

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

  Hector::IO::MADX parser( argv[1], ip, +1, s_pos );
  parser.printInfo();

  const CLHEP::Hep2Vector offset( -0.097, 0. );
  //parser.beamline()->offsetElementsAfter( 120., offset );
  //parser.romanPots()

  Hector::Propagator prop( parser.beamline() );

  //TH2D hitmap( "hitmap", "x (m)\\y (m)", 200, -0.01, 0.01, 200, -0.01, 0.01 );
  //TH2D hitmap( "hitmap", "x (m)\\y (m)", 200, -0.005, 0.005, 200, -0.005, 0.005 );
//  TH2D hitmap( "hitmap", "x (m)\\y (m)", 200, -0.25, 0.25, 200, -0.05, 0.05 );
  TH2D hitmap( "hitmap", "x (m)\\y (m)", 200, 0.08, 0.12, 200, -0.05, 0.05 );

  const unsigned int num_particles = 5000;
  const float beam_lateral_width_ip = 16.63e-6, // in meters
              beam_angular_divergence_ip = 30.23e-6, // in radians
              particles_energy = 6500.; // in GeV
  Hector::BeamProducer::gaussianParticleGun gun;
  //gun.setElimits( particles_energy*0.95, particles_energy );
  gun.setElimits( particles_energy );
  gun.setXparams( 0., beam_lateral_width_ip );
  gun.setYparams( 0., beam_lateral_width_ip );
  gun.setTXparams( Hector::Parameters::get()->crossingAngleX()/2., beam_angular_divergence_ip );
  gun.setTYparams( Hector::Parameters::get()->crossingAngleY()/2., beam_angular_divergence_ip );
  //Hector::BeamProducer::Xscanner gun( num_particles, Hector::Parameters::get()->beamEnergy(), 0., 0.01 );

  unsigned short num_stopped = 0;
  for ( size_t i=0; i!=num_particles; i++ ) {

    if ( (int)(i*(double)num_particles/num_particles)%1000==0 ) std::cout << ">>> Generating particle " << i << " / " << num_particles << std::endl;

    // propagation through the beamline
    Hector::Particle p = gun.shoot();
    try {
      prop.propagate( p, s_pos );
      //p.dump();
      if ( prop.stopped( p, s_pos ) ) { /*std::cout << "prout" << std::endl;*/ num_stopped++; continue; }
      const CLHEP::Hep2Vector pos( p.stateVectorAt( s_pos ).position()-offset );
      std::cout << s_pos << " -> " << pos << std::endl;
      hitmap.Fill( pos.x(), pos.y() );
    } catch ( Hector::Exception& e ) {
      e.dump();
      continue;
    }
  }
  PrintInfo( Form( "%.1f%% of particles (%d/%d) stopped before s = %.2f", 100. * num_stopped/num_particles, num_stopped, num_particles, s_pos ) );

  {
    Hector::Canvas c( "hitmap", Form( "Hector 2 simulation, s = %.2f m", s_pos ) );
    hitmap.Draw( "colz" );
    c.Prettify( &hitmap );

    const std::string file( argv[1] );
    Hector::Canvas::PaveText( 0.01, 0., 0.05, 0.05, Form( "#scale[0.3]{Beamline: %s}", file.substr( file.find_last_of( "/\\" )+1 ).c_str() ) ).Draw();

    c.Save( "pdf" );
  }

  return 0;
}
