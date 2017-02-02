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
  // general plotting parameters
  const float max_s = ( argc>3 ) ? atof( argv[3] ) : 500.;
  const unsigned int num_particles = 200;

  Hector::Parser::MADX parser_beam1( argv[1], "IP5", +1, max_s ),
                       parser_beam2( argv[2], "IP5", +1, max_s );

parser_beam1.beamline()->dump();
  // look at both the beamlines

  Hector::Propagator prop1( parser_beam1.beamline() ),
                     prop2( parser_beam2.beamline() );

  const double mass = Hector::Constants::beam_particles_mass,
               energy = Hector::Constants::beam_energy;
  const CLHEP::Hep3Vector mom0( 0, 0., sqrt( energy*energy-mass*mass ) );

  TMultiGraph mg1_x, mg2_x,
              mg1_y, mg2_y;

  Hector::Particles gun = Hector::BeamProducer::gaussianParticleGun( num_particles, Hector::Constants::beam_energy/1.2, Hector::Constants::beam_energy );
  //Hector::Particles gun = Hector::BeamProducer::TYscanner( num_particles, Hector::Constants::beam_energy, -1, 1, max_s );

  for ( Hector::Particles::iterator p=gun.begin(); p!=gun.end(); p++ ) {
    { // beamline 1 propagation
      p->clear();
      try { prop1.propagate( *p, max_s ); } catch ( Hector::Exception& e ) { e.dump(); }
      TGraph gr_x, gr_y;
      unsigned short j = 0;
      for ( Hector::Particle::PositionsMap::const_iterator it=p->begin(); it!=p->end(); it++, j++ ) {
        gr_x.SetPoint( j, it->first, it->second.position().x() );
        gr_y.SetPoint( j, it->first, it->second.position().y() );
        //std::cout << "--> " << it->second.position() << std::endl;
      }
      gr_x.SetLineColor( kBlack );
      gr_y.SetLineColor( kBlack );
      mg1_x.Add( dynamic_cast<TGraph*>( gr_x.Clone() ) );
      mg1_y.Add( dynamic_cast<TGraph*>( gr_y.Clone() ) );
    }
    { // beamline 2 propagation
      p->clear();
      try { prop2.propagate( *p, max_s ); } catch ( Hector::Exception& e ) { e.dump(); }

      TGraph gr_x, gr_y;
      unsigned short j = 0;
      for ( Hector::Particle::PositionsMap::const_iterator it=p->begin(); it!=p->end(); it++, j++ ) {
        gr_x.SetPoint( j, it->first, it->second.position().x() );
        gr_y.SetPoint( j, it->first, it->second.position().y() );
      }
      gr_x.SetLineColor( kRed );
      gr_y.SetLineColor( kRed );
      mg2_x.Add( dynamic_cast<TGraph*>( gr_x.Clone() ) );
      mg2_y.Add( dynamic_cast<TGraph*>( gr_y.Clone() ) );
    }
  }

  // drawing part

  Hector::Canvas c( "beamline", Form( "Hector (v2) simulation, E_{beam} = %.1f TeV", Hector::Constants::beam_energy*CLHEP::GeV/CLHEP::TeV ), true );

  c.cd( 1 ); // x-axis
  mg1_x.SetTitle( ".\\x (m)" );
  mg1_x.Draw( "al" );
  mg2_x.Draw( "l" );
  mg1_x.GetXaxis()->SetLimits( 0., max_s );
  mg1_x.GetYaxis()->SetRangeUser( -0.1, 0.1 );
  drawBeamline( 'x', parser_beam1.beamline(), 0, max_s, "IP5" );
  drawBeamline( 'x', parser_beam2.beamline(), 1, max_s, "IP5" );
  mg1_x.Draw( "lp" );
  mg2_x.Draw( "lp" );
  c.Prettify( mg1_x.GetHistogram() );
  mg1_x.GetXaxis()->SetTitle( "" );

  c.cd( 2 ); // y-axis
  mg1_y.SetTitle( "s (m)\\y (m)" );
  mg1_y.Draw( "al" );
  mg2_y.Draw( "l" );
  mg1_y.GetXaxis()->SetLimits( 0., max_s );
  mg1_y.GetYaxis()->SetRangeUser( -0.1, 0.1 );
  drawBeamline( 'y', parser_beam1.beamline(), 0, max_s, "IP5" );
  drawBeamline( 'y', parser_beam2.beamline(), 1, max_s, "IP5" );
  mg1_y.Draw( "lp" );
  mg2_y.Draw( "lp" );
  c.Prettify( mg1_y.GetHistogram() );

  //c.SetMargin( 5., 5., 5., 5. );

  c.cd();
  const std::string file1( argv[1] ), file2( argv[2] );
  Hector::Canvas::PaveText bl( 0.01, 0., 0.05, 0.05, Form( "#scale[0.3]{Beam 1: %s - Beam 2: %s}", file1.substr( file1.find_last_of( "/\\" )+1 ).c_str(), file2.substr( file2.find_last_of( "/\\")+1 ).c_str() ) );
  bl.Draw();

  c.Save( "pdf" );

  return 0;
}
