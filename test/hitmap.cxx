#include "Hector/Beamline/Beamline.h"
#include "Hector/Core/Exception.h"
#include "Hector/IO/MADXHandler.h"
#include "Hector/Propagator/Propagator.h"
#include "Hector/Utils/BeamProducer.h"

#include "utils.h"
#include "Canvas.h"

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Random/RandGauss.h>

#include "TH2.h"
#include "TH1.h"

using namespace std;

int
main( int argc, char* argv[] )
{
  if ( argc < 3 ) {
    cout << "Usage: " << argv[0] << " [MAD-X output file] [s position] <IP name> <crossing angle (x)>" << endl;
    return -1;
  }
  const float s_pos = atof( argv[2] );
  const char* ip = ( argc > 3 ) ? argv[3] : "IP5";
  const double crossing_angle_x = ( argc > 4 ) ? atof( argv[4] ) : 180.e-6;
  const double crossing_angle_y = 0.;

  Hector::IO::MADX parser( argv[1], ip, +1, s_pos );
  parser.printInfo();

  const CLHEP::Hep2Vector offset( -0.097, 0. );
  //parser.beamline()->offsetElementsAfter( 120., offset );
  //parser.romanPots()

  Hector::Propagator prop( parser.beamline() );

  //TH2D hitmap( "hitmap", "x (m)\\y (m)", 200, -0.01, 0.01, 200, -0.01, 0.01 );
  //TH2D hitmap( "hitmap", "x (m)\\y (m)", 200, -0.005, 0.005, 200, -0.005, 0.005 );
  TH2D hitmap( "hitmap", "x (m)\\y (m)", 200, 0., 0.2, 200, -0.05, 0.05 );
  //TH2D hitmap( "hitmap", "x (m)\\y (m)", 200, 0.08, 0.12, 200, -0.05, 0.05 );

  const unsigned int num_particles = 5000;
  const float beam_lateral_width_ip = 16.63e-6, // in meters
              beam_angular_divergence_ip = 30.23e-6, // in radians
              particles_energy = 6500.; // in GeV
  Hector::BeamProducer::gaussianParticleGun gun;
  //gun.setElimits( particles_energy*0.95, particles_energy );
  gun.setElimits( particles_energy );
  gun.setXparams( 0., beam_lateral_width_ip );
  gun.setYparams( 0., beam_lateral_width_ip );
  gun.setTXparams( crossing_angle_x, beam_angular_divergence_ip );
  gun.setTYparams( crossing_angle_y, beam_angular_divergence_ip );
  //Hector::BeamProducer::Xscanner gun( num_particles, Hector::Parameters::get()->beamEnergy(), 0., 0.01 );

  unsigned short num_stopped = 0;
  for ( size_t i = 0; i < num_particles; ++i ) {

    if ( (int)(i*(double)num_particles/num_particles)%1000==0 ) cout << ">>> Generating particle " << i << " / " << num_particles << endl;

    // propagation through the beamline
    Hector::Particle p = gun.shoot();
    try {
      prop.propagate( p, s_pos );
      //p.dump();
      if ( prop.stopped( p, s_pos ) ) { /*cout << "prout" << endl;*/ num_stopped++; continue; }
      const CLHEP::Hep2Vector pos( p.stateVectorAt( s_pos ).position()-offset );
      cout << s_pos << " -> " << pos << endl;
      hitmap.Fill( pos.x(), pos.y() );
    } catch ( Hector::Exception& e ) {
      e.dump();
      num_stopped++;
      continue;
    }
  }
  PrintInfo( Form( "%.1f%% of particles (%d/%d) stopped before s = %.2f", 100. * num_stopped/num_particles, num_stopped, num_particles, s_pos ) );

  {
    Hector::Canvas c( "hitmap", Form( "s = %.2f m, #alpha_{X} = %.1f #murad", s_pos, crossing_angle_x*1.e6 ) );
    hitmap.Draw( "colz" );
    c.Prettify( &hitmap );

    const string file( argv[1] );
    Hector::Canvas::PaveText( 0.01, 0., 0.05, 0.05, Form( "#scale[0.3]{Beamline: %s}", file.substr( file.find_last_of( "/\\" )+1 ).c_str() ) ).Draw();

    c.Save( "pdf" );
  }

  return 0;
}
