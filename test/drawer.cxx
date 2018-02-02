#include "Hector/Beamline/Beamline.h"
#include "Hector/IO/MADXHandler.h"
#include "Hector/Core/ParticleStoppedException.h"
#include "Hector/Propagator/Propagator.h"

#include "Hector/Utils/ArgsParser.h"
#include "Hector/Utils/BeamProducer.h"
#include "Hector/Utils/Timer.h"

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
  vector<string> twiss_filenames;
  string ip_name;
  double crossing_angle_x, crossing_angle_y, max_s;
  double beam_lateral_width_ip, beam_angular_divergence_ip;
  double scale_x, scale_y;
  unsigned int num_particles;

  Hector::ArgsParser args( argc, argv, {
    { "--twiss-files", "Twiss file(s)", &twiss_filenames },
  }, {
    { "--num-parts", "number of particles to generate", 1000, &num_particles },
    { "--ip-name", "name of the interaction point", "IP5", &ip_name },
    { "--xingangle-x", "crossing angle in the x direction (rad)", 180.e-6, &crossing_angle_x },
    { "--xingangle-y", "crossing angle in the y direction (rad)", 0., &crossing_angle_y },
    { "--scale-x", "Horizontal coordinate scaling (m)", 0.1, &scale_x },
    { "--scale-y", "Vertical coordinate scaling (m)", 0.05, &scale_y },
    { "--beam-divergence", "Beam angular divergence (rad)", 30.23e-6, &beam_angular_divergence_ip },
    { "--beam-width", "Beam transverse width (m)", 16.63e-6, &beam_lateral_width_ip },
    { "--max-s", "maximal s-coordinate (m)", 250., &max_s }
  } );

  //--- general plotting parameters
  //Hector::Parameters::get()->setUseRelativeEnergy( true ); //FIXME
  //Hector::Parameters::get()->setComputeApertureAcceptance( false ); //FIXME
  //Hector::Parameters::get()->setEnableKickers( false ); //FIXME

  //--- define the propagator objects
  vector<Hector::Propagator> propagators;

  for ( const auto& fn : twiss_filenames ) {
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
  vector<TMultiGraph*> mg_x( propagators.size(), new TMultiGraph() ), mg_y( propagators.size(), new TMultiGraph() );

  //parser_beam1.beamline()->offsetElementsAfter( 120., CLHEP::Hep2Vector( -0.097, 0. ) );
  //parser_beam2.beamline()->offsetElementsAfter( 120., CLHEP::Hep2Vector( +0.097, 0. ) );

  const double mass = Hector::Parameters::get()->beamParticlesMass(),
               energy = Hector::Parameters::get()->beamEnergy();
  const CLHEP::Hep3Vector mom0( 0, 0., sqrt( energy*energy-mass*mass ) );

  Hector::BeamProducer::GaussianParticleGun gun;
  gun.smearX( 0., beam_lateral_width_ip );
  gun.smearY( 0., beam_lateral_width_ip );
  //gun.smearTx( crossing_angle_x, beam_angular_divergence_ip );
  gun.smearTy( crossing_angle_y, beam_angular_divergence_ip );
  //Hector::BeamProducer::TYscanner gun( num_particles, Hector::Parameters::get()->beamEnergy(), -1, 1, max_s );

  Hector::Timer tmr;
  TH1D h_timing( "timing", "Propagation time\\Event\\ms?.2f", 100, 0., 10. );

  for ( size_t i = 0; i < num_particles; ++i ) {
    unsigned short j = 0;
    for ( const auto& prop : propagators ) {
      gun.smearTx( ( j == 0 ? -1 : +1 )*crossing_angle_x, beam_angular_divergence_ip );
      Hector::Particle p = gun.shoot();
      //----- beamline propagation
      TGraph gr_x, gr_y; // 1 graph for each trajectory
      try {
        tmr.reset();
        prop.propagate( p, max_s );
        const float prop_time = tmr.elapsed()*1.e6;
        h_timing.Fill( prop_time*1.e-3 ); // in ms
      } catch ( Hector::ParticleStoppedException& e ) { }
      unsigned short k = 0;
      for ( const auto& pos : p ) {
        gr_x.SetPoint( k, pos.first, pos.second.position().x() );
        gr_y.SetPoint( k, pos.first, pos.second.position().y() );
        k++;
      }
      gr_x.SetLineColor( ( j == 0 ) ? kBlack : kRed );
      gr_y.SetLineColor( ( j == 0 ) ? kBlack : kRed );
      gr_x.SetLineStyle( ( j == 0 ) ? 1 : 2 );
      gr_y.SetLineStyle( ( j == 0 ) ? 1 : 2 );
      mg_x[j]->Add( dynamic_cast<TGraph*>( gr_x.Clone() ) );
      mg_y[j]->Add( dynamic_cast<TGraph*>( gr_y.Clone() ) );
      ++j;
    }
  }

  // drawing part

  {
    Hector::Canvas c( "beamline", Form( "E_{p} = %.1f TeV, #alpha_{X} = %.1f #murad", Hector::Parameters::get()->beamEnergy()*CLHEP::GeV/CLHEP::TeV, crossing_angle_x*1.e6 ), true );
    //c.SetGrayscale();

    const bool draw_apertures = true;

    c.cd( 1 ); // x-axis
    if ( mg_x.size() > 0 ) {
      auto& first = mg_x[0];
      first->SetTitle( ".\\x (m)" );
      first->Draw( "a" );
      first->GetXaxis()->SetLimits( 0., max_s );
      first->GetYaxis()->SetRangeUser( -scale_x, scale_x );
      c.Prettify( first->GetHistogram() );

      unsigned short i = 0;
      for ( const auto& prop : propagators )
        drawBeamline( 'x', prop.beamline(), i++, "IP5", scale_x, 0., max_s, draw_apertures );
      for ( auto& mg : mg_x ) mg->Draw( "l" );

      first->GetXaxis()->SetLabelSize( 0.055 );
      first->GetYaxis()->SetLabelSize( 0.055 );
      first->GetYaxis()->SetTitleSize( 0.1 );
      first->GetYaxis()->SetTitleOffset( 0.65 );
      first->GetXaxis()->SetTitle( "" );
    }
    c.GetPad( 1 )->SetGrid( 1, 1 );

    c.cd( 2 ); // y-axis
    if ( mg_y.size() > 0 ) {
      auto& first = *mg_y.begin();
      first->SetTitle( "s (m)\\y (m)" );
      first->Draw( "a" );
      first->GetXaxis()->SetLimits( 0., max_s );
      first->GetYaxis()->SetRangeUser( -scale_y, scale_y );
      c.Prettify( first->GetHistogram() );

      unsigned short i = 0;
      for ( const auto& prop : propagators )
        drawBeamline( 'y', prop.beamline(), i++, "IP5", scale_y, 0., max_s, draw_apertures );
      for ( auto& mg : mg_y ) mg->Draw( "l" );

      first->GetXaxis()->SetLabelSize( 0.055 );
      first->GetYaxis()->SetLabelSize( 0.055 );
      first->GetXaxis()->SetTitleSize( 0.1 );
      first->GetXaxis()->SetTitleOffset( 0.7 );
      first->GetYaxis()->SetTitleSize( 0.1 );
      first->GetYaxis()->SetTitleOffset( 0.65 );
    }
    c.GetPad( 2 )->SetGrid( 1, 1 );
    //c.SetMargin( 5., 5., 5., 5. );

    c.cd();
    ostringstream os;
    unsigned short i = 1;
    for ( const auto& fn : twiss_filenames ) {
      os << ( ( i == 1 ) ? "" : " - " )
         << "Beam " << i << ": "
         << Form( "%s", fn.substr( fn.find_last_of( "/\\" )+1 ).c_str() );
      ++i;
    }
    const char* label = Form( "#scale[0.3]{%s}", os.str().c_str() );
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
