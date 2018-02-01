#include "Hector/Beamline/Beamline.h"
#include "Hector/IO/MADXHandler.h"
#include "Hector/Core/ParticleStoppedException.h"
#include "Hector/Core/Timer.h"
#include "Hector/Propagator/Propagator.h"

#include "Hector/Utils/ArgsParser.h"
#include "Hector/Utils/BeamProducer.h"

#include "utils.h"
#include "Canvas.h"

#include <CLHEP/Random/RandGauss.h>
#include <CLHEP/Units/SystemOfUnits.h>

#include "TGraph.h"
#include "TMultiGraph.h"
#include "TAxis.h"
#include "TStyle.h"

using namespace std;

int
main( int argc, char* argv[] )
{
  string twiss1_filename, twiss2_filename, ip_name;
  double crossing_angle_x, crossing_angle_y, max_s;
  double scale_x, scale_y;
  unsigned int num_particles;

  Hector::ArgsParser args( argc, argv, {
    { "--twiss1", "first Twiss file", &twiss1_filename },
  }, {
    { "--twiss2", "second Twiss file", "", &twiss2_filename },
    { "--num-parts", "number of particles to generate", 1000, &num_particles },
    { "--ip-name", "name of the interaction point", "IP5", &ip_name },
    { "--xingangle-x", "crossing angle in the x direction (rad)", 180.e-6, &crossing_angle_x },
    { "--xingangle-y", "crossing angle in the y direction (rad)", 0., &crossing_angle_y },
    { "--scale-x", "Horizontal coordinate scaling (m)", 0.1, &scale_x },
    { "--scale-y", "Vertical coordinate scaling (m)", 0.05, &scale_y },
    { "--max-s", "maximal s-coordinate (m)", 250., &max_s }
  } );

  //--- general plotting parameters
  //Hector::Parameters::get()->setUseRelativeEnergy( true ); //FIXME
  //Hector::Parameters::get()->setComputeApertureAcceptance( false ); //FIXME
  //Hector::Parameters::get()->setEnableKickers( false ); //FIXME

  //--- define the propagator objects
  vector<Hector::Propagator> propagators;

  for ( const auto& fn : vector<string>{ { twiss1_filename, twiss2_filename } } ) {
    if ( fn == "" ) continue;
    const Hector::IO::MADX parser( fn, ip_name, 1, max_s );
    auto bl = new Hector::Beamline( *parser.beamline() );
    propagators.emplace_back( bl );

    //--- look at the beamline(s)
    //bl->dump();
    auto rps = bl->find( "XRPH\\." );
    cout << "---> beamline " << fn << " has " << rps.size() << " roman pots!" << endl;
    for ( const auto& rp : rps )
      cout << " >> Roman pot " << rp->name() << " at s=" << rp->s() << " m" << endl;
  }
  vector<TMultiGraph> mg_x( propagators.size() ), mg_y( propagators.size() );

  //parser_beam1.beamline()->offsetElementsAfter( 120., CLHEP::Hep2Vector( -0.097, 0. ) );
  //parser_beam2.beamline()->offsetElementsAfter( 120., CLHEP::Hep2Vector( +0.097, 0. ) );

  const double mass = Hector::Parameters::get()->beamParticlesMass(),
               energy = Hector::Parameters::get()->beamEnergy();
  const CLHEP::Hep3Vector mom0( 0, 0., sqrt( energy*energy-mass*mass ) );

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

  for ( size_t i = 0; i < num_particles; ++i ) {
    gun.setTXparams( -crossing_angle_x, beam_angular_divergence_ip );
    Hector::Particle p = gun.shoot();
    unsigned short j = 0;
    for ( const auto& prop : propagators ) { //----- beamline propagation
      TGraph gr_x, gr_y;
      try {
        tmr.reset();
        prop.propagate( p, max_s );
        const float prop_time = tmr.elapsed()*1.e6;
        h_timing.Fill( prop_time ); // in us
      } catch ( Hector::ParticleStoppedException& e ) { continue; }
      unsigned short k = 0;
      for ( const auto& pos : p ) {
        gr_x.SetPoint( k, pos.first, pos.second.position().x() );
        gr_y.SetPoint( k, pos.first, pos.second.position().y() );
        k++;
      }
      gr_x.SetLineColor( ( j == 0 ) ? kBlack : kRed );
      gr_y.SetLineColor( ( j == 0 ) ? kBlack : kRed );
      mg_x[j].Add( dynamic_cast<TGraph*>( gr_x.Clone() ) );
      mg_y[j].Add( dynamic_cast<TGraph*>( gr_y.Clone() ) );
      ++j;
    }
  }

  // drawing part

  {
    Hector::Canvas c( "beamline", Form( "E_{p} = %.1f TeV, #alpha_{X} = %.1f #murad", Hector::Parameters::get()->beamEnergy()*CLHEP::GeV/CLHEP::TeV, crossing_angle_x*1.e6 ), true );
    //c.SetGrayscale();

    const bool draw_apertures = true;

    c.cd( 1 ); // x-axis
    {
      auto& first = *mg_x.begin();
      cout << "haha" << mg_x.size() << endl;
      first.SetTitle( ".\\x (m)" );
      first.Draw( "al" );
      first.GetXaxis()->SetLimits( 0., max_s );
      first.GetYaxis()->SetRangeUser( -scale_x, scale_x );
      //c.Prettify( first.GetHistogram() );
      unsigned short i = 0;
      for ( const auto& prop : propagators ) {
        drawBeamline( 'x', prop.beamline(), i, "IP5", scale_x, 0., max_s, draw_apertures );
        ++i;
      }

      for ( auto& mg : mg_x ) {
        mg.Draw( "l,same" );
        mg.GetXaxis()->SetLabelSize( 0.055 );
        mg.GetYaxis()->SetLabelSize( 0.055 );
        mg.GetYaxis()->SetTitleSize( 0.1 );
        mg.GetYaxis()->SetTitleOffset( 0.65 );
        mg.GetXaxis()->SetTitle( "" );
      }
    }
    c.GetPad( 1 )->SetGrid( 1, 1 );

    c.cd( 2 ); // y-axis
    {
      auto& first = *mg_y.begin();
      first.SetTitle( "s (m)\\y (m)" );
      first.Draw( "al" );
      first.GetXaxis()->SetLimits( 0., max_s );
      first.GetYaxis()->SetRangeUser( -scale_y, scale_y );
      //c.Prettify( first.GetHistogram() );
      unsigned short i = 0;
      for ( const auto& prop : propagators ) {
        drawBeamline( 'y', prop.beamline(), i, "IP5", scale_y, 0., max_s, draw_apertures );
        ++i;
      }
      for ( auto& mg : mg_y ) {
        mg.Draw( "l,same" );
        mg.GetXaxis()->SetLabelSize( 0.055 );
        mg.GetYaxis()->SetLabelSize( 0.055 );
        mg.GetYaxis()->SetTitleSize( 0.1 );
        mg.GetYaxis()->SetTitleOffset( 0.65 );
        mg.GetXaxis()->SetTitleOffset( 0.7 );
        mg.GetXaxis()->SetTitle( "" );
      }
    }
    c.GetPad( 2 )->SetGrid( 1, 1 );
    //c.SetMargin( 5., 5., 5., 5. );

    c.cd();
    const char* label = Form( "#scale[0.3]{Beam 1: %s - Beam 2: %s}",
                              twiss1_filename.substr( twiss1_filename.find_last_of( "/\\" )+1 ).c_str(),
                              twiss2_filename.substr( twiss2_filename.find_last_of( "/\\")+1 ).c_str() );
    auto pt = new Hector::Canvas::PaveText( 0.0, 0.0, 0.15, 0.01, label );
    pt->SetTextAlign( kHAlignLeft+kVAlignBottom );
    pt->Draw();

    c.Save( "pdf" );
  }
  { // draw the legend
    Hector::Canvas c( "beamline_legend" );
    elementsLegend leg;
    gStyle->SetOptStat( 0 );
    for ( const auto& prop : propagators )
      leg.feedBeamline( prop.beamline() );
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
