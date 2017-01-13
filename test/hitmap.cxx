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
    std::cout << "Usage: " << argv[0] << " [MAD-X output file] [s position] <IP name>" << std::endl;
    return -1;
  }
  const float s_pos = atof( argv[2] );
  const char* ip = ( argc>3 ) ? argv[3] : "IP5";

  Hector::Parser::MADX parser( argv[1], ip, +1, s_pos );

  // look at both the beamlines

  Hector::Propagator prop( parser.beamline() );

  const double mass = Hector::Constants::beam_particles_mass,
               energy = Hector::Constants::beam_energy;
  const CLHEP::Hep3Vector mom0( 0, 0., sqrt( energy*energy-mass*mass ) );

  TH2D hitmap( "hitmap", "x (m)\\y (m)", 1000, -0.2, 0.2, 1000, -0.2, 0.2 );
  TH1D smear_x( "smear_x", "", 100, -10., 10. ),
       smear_y( "smear_y", "", 100, -10., 10. );
  TH1D pt_ini( "pt_ini", "Initial transverse momentum\\Entries\\GeV", 100, 0., 100. ),
       px_ini( "px_ini", "Initial horizontal momentum\\Entries\\GeV", 100, 0., 100. );

  const double smearing = 1.e-3; //CLHEP::pi

  for ( unsigned int i=0; i<10000; i++ ) {

    const float rot_x = CLHEP::RandGauss::shoot( 0., smearing ),
                rot_y = CLHEP::RandGauss::shoot( 0., smearing );

    CLHEP::HepLorentzVector p0( mom0, energy );
    p0.rotateX( rot_x );
    p0.rotateY( rot_y );

    pt_ini.Fill( p0.perp() );
    px_ini.Fill( p0.px() );
    smear_x.Fill( rot_x );
    smear_y.Fill( rot_y );

    Hector::Particle p( p0 );
    p.setCharge( +1 );

    { // propagation through the beamline
      prop.propagate( p, s_pos );

      Hector::Particle::PositionsMap::const_iterator it = --p.end();
      Hector::Particle::StateVector sv = it->second;
      hitmap.Fill( sv.position().x(), sv.position().y() );
    }
    //p.dump();
  }

  {
    Canvas c( "hitmap", "" );
    hitmap.Draw( "colz" );
    c.Prettify( &hitmap );
    c.Save( "pdf" );
  }
  {
    Canvas c( "initial_pt", "" );
    pt_ini.Draw();
    c.Prettify( &pt_ini );
    c.Save( "pdf" );
  }
  {
    Canvas c( "initial_px", "" );
    px_ini.Draw();
    c.Prettify( &px_ini );
    c.Save( "pdf" );
  }
  {
    Canvas c( "smearing_param", "" );
    smear_x.Draw();
    smear_y.SetLineColor( kRed+1 );
    smear_y.Draw( "same" );
    c.Prettify( &smear_x );
    c.Save( "pdf" );
  }

  return 0;
}
