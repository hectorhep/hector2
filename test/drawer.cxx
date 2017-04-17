#include "beamline/Beamline.h"
#include "io/MADXParser.h"
#include "propagator/Propagator.h"

#include "utils.h"
#include "Canvas.h"

#include <CLHEP/Random/RandGauss.h>

#include "TGraph.h"
#include "TMultiGraph.h"
#include "TAxis.h"
#include "TStyle.h"

int
main( int argc, char* argv[] )
{
  if ( argc<3 ) {
    std::cout << "Usage: " << argv[0] << " [MAD-X output file for beam 1] [MAD-X output file for beam 2]" << std::endl;
    return -1;
  }
  // general plotting parameters
  const float max_s = ( argc>3 ) ? atof( argv[3] ) : 500.;
  const unsigned int num_particles = 1;

  Hector::Parser::MADX parser_beam1( argv[1], "IP5", +1, max_s ),
                       parser_beam2( argv[2], "IP5", +1, max_s );

  // look at both the beamlines

  const Hector::Elements rps = parser_beam1.romanPots();
  std::cout << "---> beamline 1 has " << rps.size() << " roman pots!" << std::endl;
  for ( unsigned short i=0; i<rps.size(); i++ ) {
    std::cout << " >> Roman pot " << rps[i]->name() << " at s=" << rps[i]->s() << " m" << std::endl;
  }

  //parser_beam1.beamline()->offsetElementsAfter( 120., CLHEP::Hep2Vector( +0.022, 0. ) );
  //parser_beam2.beamline()->offsetElementsAfter( 120., CLHEP::Hep2Vector( -0.022, 0. ) );

  Hector::Propagator prop1( parser_beam1.beamline() ),
                     prop2( parser_beam2.beamline() );

  const double mass = Hector::Parameters::beam_particles_mass,
               energy = Hector::Parameters::beam_energy;
  const CLHEP::Hep3Vector mom0( 0, 0., sqrt( energy*energy-mass*mass ) );

  TMultiGraph mg1_x, mg2_x,
              mg1_y, mg2_y;

  const float beam_lateral_width_ip = 16.63e-6, // in meters
              beam_angular_divergence_ip = 30.23e-6; // in radians
  Hector::BeamProducer::gaussianParticleGun gun;
  gun.setXparams( 0., beam_lateral_width_ip );
  gun.setYparams( 0., beam_lateral_width_ip );
  gun.setTXparams( Hector::Parameters::crossing_angle/2., beam_angular_divergence_ip );
  gun.setTYparams( 0., beam_angular_divergence_ip );
  //Hector::BeamProducer::TYscanner gun( num_particles, Hector::Parameters::beam_energy, -1, 1, max_s );

  for ( size_t i=0; i<num_particles; i++ ) {
    unsigned short j;
    Hector::Particle p = gun.shoot();
    { // beamline 1 propagation
      p.clear();
      TGraph gr_x, gr_y;
      try { prop1.propagate( p, max_s ); } catch ( Hector::Exception& e ) { e.dump(); }
      j = 0;
      for ( Hector::Particle::PositionsMap::const_iterator it=p.begin(); it!=p.end(); it++, j++ ) {
        gr_x.SetPoint( j, it->s(), it->stateVector().position().x() );
        gr_y.SetPoint( j, it->s(), it->stateVector().position().y() );
      }
      gr_x.SetLineColor( kBlack );
      gr_y.SetLineColor( kBlack );
      mg1_x.Add( dynamic_cast<TGraph*>( gr_x.Clone() ) );
      mg1_y.Add( dynamic_cast<TGraph*>( gr_y.Clone() ) );
    }
    { // beamline 2 propagation
      p.clear();
      TGraph gr_x, gr_y;
      try { prop2.propagate( p, max_s ); } catch ( Hector::Exception& e ) { e.dump(); }
      j = 0;
      for ( Hector::Particle::PositionsMap::const_iterator it=p.begin(); it!=p.end(); it++, j++ ) {
        gr_x.SetPoint( j, it->s(), it->stateVector().position().x() );
        gr_y.SetPoint( j, it->s(), it->stateVector().position().y() );
      }
      gr_x.SetLineColor( kRed );
      gr_x.SetLineStyle( 2 );
      gr_y.SetLineColor( kRed );
      gr_y.SetLineStyle( 2 );
      mg2_x.Add( dynamic_cast<TGraph*>( gr_x.Clone() ) );
      mg2_y.Add( dynamic_cast<TGraph*>( gr_y.Clone() ) );
    }
  }

  // drawing part

  {
    Hector::Canvas c( "beamline", Form( "Hector 2 simulation, E_{beam} = %.1f TeV", Hector::Parameters::beam_energy*CLHEP::GeV/CLHEP::TeV ), true );
    //c.SetGrayscale();

    c.cd( 1 ); // x-axis
    mg1_x.SetTitle( ".\\x (m)" );
    mg1_x.Draw( "al" );
    mg2_x.Draw( "l" );
    mg1_x.GetXaxis()->SetLimits( 0., max_s );
    mg1_x.GetYaxis()->SetRangeUser( -0.2, 0.2 );
    drawBeamline( 'x', parser_beam1.beamline(), 0, "IP5", 0., max_s );
    drawBeamline( 'x', parser_beam2.beamline(), 1, "IP5", 0., max_s );
    mg1_x.Draw( "l" );
    mg2_x.Draw( "l" );
    c.Prettify( mg1_x.GetHistogram() );
    mg1_x.GetXaxis()->SetTitle( "" );

    c.cd( 2 ); // y-axis
    mg1_y.SetTitle( "s (m)\\y (m)" );
    mg1_y.Draw( "al" );
    mg2_y.Draw( "l" );
    mg1_y.GetXaxis()->SetLimits( 0., max_s );
    mg1_y.GetYaxis()->SetRangeUser( -0.2, 0.2 );
    drawBeamline( 'y', parser_beam1.beamline(), 0, "IP5", 0., max_s );
    drawBeamline( 'y', parser_beam2.beamline(), 1, "IP5", 0., max_s );
    mg1_y.Draw( "l" );
    mg2_y.Draw( "l" );
    c.Prettify( mg1_y.GetHistogram() );

    //c.SetMargin( 5., 5., 5., 5. );

    c.cd();
    const std::string file1( argv[1] ), file2( argv[2] );
    Hector::Canvas::PaveText( 0.01, 0., 0.05, 0.05, Form( "#scale[0.3]{Beam 1: %s - Beam 2: %s}", file1.substr( file1.find_last_of( "/\\" )+1 ).c_str(), file2.substr( file2.find_last_of( "/\\")+1 ).c_str() ) ).Draw();

    c.Save( "pdf" );
  }
  { // draw the legend
    Hector::Canvas c( "beamline_legend" );
    elementsLegend leg;
    gStyle->SetOptStat( 0 );
    leg.feedBeamline( parser_beam1.beamline() );
    leg.feedBeamline( parser_beam2.beamline() );
    leg.Draw();
    c.Save( "pdf" );
  }

  return 0;
}
