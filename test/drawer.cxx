#include "beamline/Beamline.h"
#include "io/MADXParser.h"
#include "propagator/Propagator.h"

#include "utils.h"
#include "Canvas.h"

#include <CLHEP/Random/RandGauss.h>

#include "TGraph.h"
#include "TMultiGraph.h"
#include "TAxis.h"

int
main( int argc, char* argv[] )
{
  if ( argc<3 ) {
    std::cout << "Usage: " << argv[0] << " [MAD-X output file for beam 1] [MAD-X output file for beam 2]" << std::endl;
    return -1;
  }
  const char* ip = ( argc>3 ) ? argv[3] : "IP5";

  // general plotting parameters
  const float max_s = 500.;
  const double smearing = 1.e-1; // rad

  Canvas c( "beamline", argv[1], true );
  
  Parser::MADX parser_beam1( argv[1], ip, +1, max_s ),
               parser_beam2( argv[2], ip, +1, max_s );

  // look at both the beamlines

  Propagator prop1( parser_beam1.beamline() ),
             prop2( parser_beam2.beamline() );

  const double mass = Constants::beam_particles_mass,
               energy = Constants::beam_energy;  
  const CLHEP::Hep3Vector mom0( 0, 0., sqrt( energy*energy-mass*mass ) );

  TMultiGraph mg1_x, mg2_x,
              mg1_y, mg2_y;

  for ( unsigned int i=0; i<1; i++ ) {

    const float rot_x = CLHEP::RandGauss::shoot( 0., smearing ),
                rot_y = CLHEP::RandGauss::shoot( 0., smearing );

    CLHEP::HepLorentzVector p0( mom0, energy );
    p0.rotateX( rot_x );
    p0.rotateY( rot_y );

    Particle p( p0 );
    p.setCharge( +1 );

    { // beamline 1 propagation
      prop1.propagate( p, max_s );

      TGraph gr_x, gr_y;
      unsigned short j = 0;
      for ( Particle::PositionsMap::const_iterator it=p.begin(); it!=p.end(); it++, j++ ) {
        gr_x.SetPoint( j, it->first, it->second.position().x() );
        gr_y.SetPoint( j, it->first, it->second.position().y() );
      }
      gr_x.SetLineColor( kBlack );
      gr_y.SetLineColor( kBlack );
      mg1_x.Add( dynamic_cast<TGraph*>( gr_x.Clone() ) );
      mg1_y.Add( dynamic_cast<TGraph*>( gr_y.Clone() ) );
    }
    { // beamline 2 propagation
      prop2.propagate( p, max_s );

      TGraph gr_x, gr_y;
      unsigned short j = 0;
      for ( Particle::PositionsMap::const_iterator it=p.begin(); it!=p.end(); it++, j++ ) {
        gr_x.SetPoint( j, it->first, it->second.position().x() );
        gr_y.SetPoint( j, it->first, it->second.position().y() );
      }
      gr_x.SetLineColor( kRed );
      gr_y.SetLineColor( kRed );
      mg2_x.Add( dynamic_cast<TGraph*>( gr_x.Clone() ) );
      mg2_y.Add( dynamic_cast<TGraph*>( gr_y.Clone() ) );
    }
    //p.dump();
  }

  c.cd( 1 ); // x-axis
  mg1_x.SetTitle( ".\\x (m)" );
  mg1_x.Draw( "al" );
  mg2_x.Draw( "l" );
  mg1_x.GetYaxis()->SetRangeUser( -0.1, 0.1 );
  drawBeamline( 'x', parser_beam1.beamline(), 0, max_s, ip );
  drawBeamline( 'x', parser_beam2.beamline(), 1, max_s, ip );
  mg1_x.Draw( "lp" );
  mg2_x.Draw( "lp" );
  c.Prettify( mg1_x.GetHistogram() );

  c.cd( 2 ); // y-axis
  mg1_y.SetTitle( "s (m)\\y (m)" );
  mg1_y.Draw( "al" );
  mg2_y.Draw( "l" );
  mg1_y.GetYaxis()->SetRangeUser( -0.1, 0.1 );
  drawBeamline( 'y', parser_beam1.beamline(), 0, max_s, ip );
  drawBeamline( 'y', parser_beam2.beamline(), 1, max_s, ip );
  mg1_y.Draw( "lp" );
  mg2_y.Draw( "lp" );
  c.Prettify( mg1_y.GetHistogram() );

  //c.SetMargin( 5., 5., 5., 5. );
  c.Save( "pdf" );

  return 0;
}
