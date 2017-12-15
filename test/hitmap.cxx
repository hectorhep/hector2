#include "Hector/Beamline/Beamline.h"
#include "Hector/Core/Exception.h"
#include "Hector/IO/MADXHandler.h"
#include "Hector/Propagator/Propagator.h"
#include "Hector/Utils/BeamProducer.h"
#include "Hector/Utils/ArgsParser.h"

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
  string twiss_filename, interaction_point;
  unsigned int num_particles;
  double crossing_angle_x, crossing_angle_y;
  double beam_angular_divergence_ip, beam_lateral_width_ip, particles_energy;
  double s_pos;

  Hector::ArgsParser args( argc, argv, {
    { "--twiss-file", "MAD-X Twiss file", &twiss_filename },
    { "--s-pos", "s-coordinate (m)", &s_pos }
  }, {
    { "--num-parts", "number of particles to generate", 1000, &num_particles },
    { "--ip-name", "name of the interaction point", "IP5", &interaction_point },
    { "--xingangle-x", "crossing angle in the x direction (rad)", 180.e-6, &crossing_angle_x },
    { "--xingangle-y", "crossing angle in the y direction (rad)", 0., &crossing_angle_y },
    { "--angular-divergence", "beam angular divergence at the interaction point (rad)", 30.23e-6, &beam_angular_divergence_ip },
    { "--beam-width", "beam lateral width at the interaction point (m)", 13.63e-6, &beam_lateral_width_ip },
    { "--particles-energy", "beam particles energy (GeV)", 6500., &particles_energy }
  } );
  Hector::IO::MADX parser( twiss_filename, interaction_point, +1, s_pos );
  parser.printInfo();

  const CLHEP::Hep2Vector offset( -0.097, 0. );
  //parser.beamline()->offsetElementsAfter( 120., offset );
  //parser.romanPots()

  Hector::Propagator prop( parser.beamline() );

  //TH2D hitmap( "hitmap", "x (m)\\y (m)", 200, -0.01, 0.01, 200, -0.01, 0.01 );
  //TH2D hitmap( "hitmap", "x (m)\\y (m)", 200, -0.005, 0.005, 200, -0.005, 0.005 );
  TH2D hitmap( "hitmap", "x (m)\\y (m)", 200, 0., 0.2, 200, -0.05, 0.05 );
  //TH2D hitmap( "hitmap", "x (m)\\y (m)", 200, 0.08, 0.12, 200, -0.05, 0.05 );

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

    Hector::Canvas::PaveText( 0.01, 0., 0.05, 0.05, Form( "#scale[0.3]{Beamline: %s}", twiss_filename.substr( twiss_filename.find_last_of( "/\\" )+1 ).c_str() ) ).Draw();

    c.Save( "pdf" );
  }

  return 0;
}
