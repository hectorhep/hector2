#include "Hector/Beamline/Beamline.h"
#include "Hector/IO/MADXHandler.h"
#include "Hector/Core/Exception.h"
#include "Hector/Core/Timer.h"
#include "Hector/Propagator/BeamProducer.h"
#include "Hector/Propagator/Propagator.h"

#include "utils.h"
#include "Canvas.h"

#include <CLHEP/Random/RandGauss.h>
#include <CLHEP/Units/SystemOfUnits.h>

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
  Hector::Parameters::get()->setUseRelativeEnergy( false ); //FIXME
  //Hector::Parameters::get()->setComputeApertureAcceptance( false ); //FIXME

  //--- general plotting parameters
  const float max_s = ( argc > 3 ) ? atof( argv[3] ) : 250.;
  const unsigned int num_particles = 1000;

  const float crossing_angle_x = 180.e-6, crossing_angle_y = 0.;

  const Hector::IO::MADX parser_beam1( argv[1], "IP5", +1, max_s ), parser_beam2( argv[2], "IP5", -1, max_s );

  //--- look at both the beamlines
  //parser_beam1.beamline()->dump();
  //parser_beam2.beamline()->dump();

  const Hector::Elements rps1 = parser_beam1.romanPots(), rps2 = parser_beam2.romanPots();
  std::cout << "---> beamline 1 has " << rps1.size() << " roman pots!" << std::endl;
  for ( unsigned short i = 0; i < rps1.size(); ++i ) {
    std::cout << " >> Roman pot " << rps1[i]->name() << " at s=" << rps1[i]->s() << " m" << std::endl;
  }
  std::cout << "---> beamline 2 has " << rps2.size() << " roman pots!" << std::endl;
  for ( unsigned short i = 0; i < rps2.size(); ++i ) {
    std::cout << " >> Roman pot " << rps2[i]->name() << " at s=" << rps2[i]->s() << " m" << std::endl;
  }

  //parser_beam1.beamline()->offsetElementsAfter( 120., CLHEP::Hep2Vector( -0.097, 0. ) );
  //parser_beam2.beamline()->offsetElementsAfter( 120., CLHEP::Hep2Vector( +0.097, 0. ) );

  //--- define the propagator objects
  const Hector::Propagator prop1( parser_beam1.beamline() ), prop2( parser_beam2.beamline() );

  const double mass = Hector::Parameters::get()->beamParticlesMass(),
               energy = Hector::Parameters::get()->beamEnergy();
  const CLHEP::Hep3Vector mom0( 0, 0., sqrt( energy*energy-mass*mass ) );

  TMultiGraph mg1_x, mg2_x,
              mg1_y, mg2_y;

  const float beam_lateral_width_ip = 16.63e-6, // in meters
              beam_angular_divergence_ip = 30.23e-6; // in radians
  Hector::BeamProducer::gaussianParticleGun gun;
  gun.setXparams( 0., beam_lateral_width_ip );
  gun.setYparams( 0., beam_lateral_width_ip );
  gun.setTXparams( crossing_angle_x, beam_angular_divergence_ip );
  gun.setTYparams( crossing_angle_y, beam_angular_divergence_ip );
  //Hector::BeamProducer::TYscanner gun( num_particles, Hector::Parameters::get()->beamEnergy(), -1, 1, max_s );

  Hector::Timer tmr;
  TH1D h_timing( "timing", "Propagation time per element\\Event\\#mus?.2f", 100, 0., 10. );

  const unsigned short num_beamline_elems = parser_beam1.beamline()->numElements();

  for ( size_t i = 0; i < num_particles; ++i ) {
    unsigned short j;
    { //----- beamline 1 propagation
      gun.setTXparams( -crossing_angle_x, beam_angular_divergence_ip );
      Hector::Particle p = gun.shoot();
      TGraph gr_x, gr_y;
      try {
        tmr.reset();
        prop1.propagate( p, max_s );
        const float prop_time = tmr.elapsed()*1.e6/num_beamline_elems;
        h_timing.Fill( prop_time ); // in us
      } catch ( Hector::Exception& e ) { e.dump(); }
      j = 0;
      for ( const auto& pos : p ) {
        gr_x.SetPoint( j, pos.first, pos.second.position().x() );
        gr_y.SetPoint( j, pos.first, pos.second.position().y() );
        j++;
      }
      gr_x.SetLineColor( kBlack );
      gr_y.SetLineColor( kBlack );
      mg1_x.Add( dynamic_cast<TGraph*>( gr_x.Clone() ) );
      mg1_y.Add( dynamic_cast<TGraph*>( gr_y.Clone() ) );
    }
    { // beamline 2 propagation
      gun.setTXparams( crossing_angle_x, beam_angular_divergence_ip );
      Hector::Particle p = gun.shoot();
      TGraph gr_x, gr_y;
      try { prop2.propagate( p, max_s ); } catch ( Hector::Exception& e ) { e.dump(); }
      j = 0;
      for ( const auto& pos : p ) {
        gr_x.SetPoint( j, pos.first, pos.second.position().x() );
        gr_y.SetPoint( j, pos.first, pos.second.position().y() );
        j++;
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
    Hector::Canvas c( "beamline", Form( "E_{p} = %.1f TeV, #alpha_{X} = %.1f #murad", Hector::Parameters::get()->beamEnergy()*CLHEP::GeV/CLHEP::TeV, crossing_angle_x*1.e6 ), true );
    //c.SetGrayscale();

    const float scale_x = 0.1,
                scale_y = 0.05;
    const bool draw_apertures = false;

    c.cd( 1 ); // x-axis
    mg1_x.SetTitle( ".\\x (m)" );
    mg1_x.Draw( "al" );
    mg2_x.Draw( "l" );
    mg1_x.GetXaxis()->SetLimits( 0., max_s );
    mg1_x.GetYaxis()->SetRangeUser( -scale_x, scale_x );
    drawBeamline( 'x', parser_beam1.beamline(), 0, "IP5", scale_x, 0., max_s, draw_apertures );
    drawBeamline( 'x', parser_beam2.beamline(), 1, "IP5", scale_x, 0., max_s, draw_apertures );
    mg1_x.Draw( "l" );
    mg2_x.Draw( "l" );
    c.Prettify( mg1_x.GetHistogram() );
    mg1_x.GetXaxis()->SetLabelSize( 0.055 );
    mg1_x.GetYaxis()->SetLabelSize( 0.055 );
    mg1_x.GetYaxis()->SetTitleSize( 0.1 );
    mg1_x.GetYaxis()->SetTitleOffset( 0.65 );
    mg1_x.GetXaxis()->SetTitle( "" );

    c.cd( 2 ); // y-axis
    mg1_y.SetTitle( "s (m)\\y (m)" );
    mg1_y.Draw( "al" );
    mg2_y.Draw( "l" );
    mg1_y.GetXaxis()->SetLimits( 0., max_s );
    mg1_y.GetYaxis()->SetRangeUser( -scale_y, scale_y );
    drawBeamline( 'y', parser_beam1.beamline(), 0, "IP5", scale_y, 0., max_s, draw_apertures );
    drawBeamline( 'y', parser_beam2.beamline(), 1, "IP5", scale_y, 0., max_s, draw_apertures );
    mg1_y.Draw( "l" );
    mg2_y.Draw( "l" );
    c.Prettify( mg1_y.GetHistogram() );
    mg1_y.GetXaxis()->SetLabelSize( 0.055 );
    mg1_y.GetYaxis()->SetLabelSize( 0.055 );
    mg1_y.GetXaxis()->SetTitleSize( 0.1 );
    mg1_y.GetYaxis()->SetTitleSize( 0.1);
    mg1_y.GetXaxis()->SetTitleOffset( 0.7 );
    mg1_y.GetYaxis()->SetTitleOffset( 0.65 );

    //c.SetMargin( 5., 5., 5., 5. );

    c.cd();
    const std::string file1( argv[1] ), file2( argv[2] );
    auto pt = new Hector::Canvas::PaveText( 0.0, 0.0, 0.15, 0.01, Form( "#scale[0.3]{Beam 1: %s - Beam 2: %s}", file1.substr( file1.find_last_of( "/\\" )+1 ).c_str(), file2.substr( file2.find_last_of( "/\\")+1 ).c_str() ) );
    pt->SetTextAlign( kHAlignLeft+kVAlignBottom );
    pt->Draw();

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
  {
    Hector::Canvas c( "propagation_time", Form( "%d events, s_{max} = %.1f m", num_particles, max_s ) );
    gStyle->SetOptStat( 1111 );
    h_timing.Draw();
    c.Prettify( &h_timing );
    c.Save( "pdf" );
  }

  return 0;
}
