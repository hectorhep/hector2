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

TGraphErrors mean_trajectory( const TMultiGraph& );

int
main( int argc, char* argv[] )
{
  vector<string> twiss_filenames;
  string ip_name;
  vector<double> crossing_angles_x, crossing_angles_y;
  double max_s;
  double beam_lateral_width_ip, beam_angular_divergence_ip;
  double xi;
  double scale_x, scale_y;
  unsigned int num_particles;
  int dir;

  Hector::ArgsParser( argc, argv, {
    { "twiss-files", "beamline(s) Twiss file(s)", &twiss_filenames, 'i' },
  }, {
    { "ip-name", "name of the interaction point", "IP5", &ip_name, 'c' },
    { "direction", "Twiss file parsing direction", +1, &dir, 'd' },
    { "max-s", "maximal s-coordinate (m)", 250., &max_s },
    { "num-parts", "number of particles to generate", 1000, &num_particles, 'n' },
    { "xi", "particles momentum loss", 0.1, &xi },
    { "alpha-x", "crossing angle in the x direction (rad)", vector<double>{ 180.e-6 }, &crossing_angles_x, 'x' },
    { "alpha-y", "crossing angle in the y direction (rad)", vector<double>{ 0. }, &crossing_angles_y, 'y' },
    { "scale-x", "Horizontal coordinate scaling (m)", 0.1, &scale_x },
    { "scale-y", "Vertical coordinate scaling (m)", 0.05, &scale_y },
    { "beam-divergence", "Beam angular divergence (rad)", 30.23e-6, &beam_angular_divergence_ip, 'r' },
    { "beam-width", "Beam transverse width (m)", 16.63e-6, &beam_lateral_width_ip, 'w' },
  } );

  for ( const auto& x : crossing_angles_x ) cout << x << endl;
  //--- general propagation parameters
  //Hector::Parameters::get()->setUseRelativeEnergy( true ); //FIXME
  //Hector::Parameters::get()->setComputeApertureAcceptance( false ); //FIXME
  //Hector::Parameters::get()->setEnableKickers( false ); //FIXME
  //Hector::Parameters::get()->setEnableDipoles( false ); //FIXME

  //--- define the propagator objects
  vector<Hector::Propagator> propagators;

  for ( const auto& fn : twiss_filenames ) {
    if ( fn == "" ) continue;
    const Hector::IO::MADX parser( fn, ip_name, dir, max_s );
    auto bl = new Hector::Beamline( *parser.beamline() );
    propagators.emplace_back( bl );

    //--- look at the beamline(s)
    bl->dump();
    auto rps = bl->find( "XRPH\\." );
    cout << "---> beamline " << fn << " has " << rps.size() << " roman pots!" << endl;
    for ( const auto& rp : rps )
      cout << " >> Roman pot " << rp->name() << " at s=" << rp->s() << " m" << endl;
  }
  TMultiGraph mg_x[2], mg_y[2];

  //parser_beam1.beamline()->offsetElementsAfter( 120., CLHEP::Hep2Vector( -0.097, 0. ) );
  //parser_beam2.beamline()->offsetElementsAfter( 120., CLHEP::Hep2Vector( +0.097, 0. ) );

  Hector::BeamProducer::GaussianParticleGun gun;
  gun.smearX( 0., beam_lateral_width_ip );
  gun.smearY( 0., beam_lateral_width_ip );

  Hector::Timer tmr;
  TH1D h_timing( "timing", "Propagation time@@Event@@ms?.2f", 100, 0., 10. );

  for ( size_t i = 0; i < num_particles; ++i ) {
    if ( num_particles > 10 && i % ( num_particles / 10 ) == 0 )
      cout << "> event " << i << " / " << num_particles << endl;

    unsigned short j = 0;
    for ( const auto& prop : propagators ) {
      const double crossing_angle_x = ( crossing_angles_x.size() == propagators.size() ) ? crossing_angles_x[j] : crossing_angles_x[0];
      const double crossing_angle_y = ( crossing_angles_y.size() == propagators.size() ) ? crossing_angles_y[j] : crossing_angles_y[0];
      gun.smearTx( crossing_angle_x, beam_angular_divergence_ip );
      gun.smearTy( crossing_angle_y, beam_angular_divergence_ip );
      Hector::Particle p = gun.shoot();
      p.firstStateVector().setXi( xi );
//      p.firstStateVector().setKick( 0. );
//      cout << p.firstStateVector().kick() << endl;
      //----- beamline propagation
      TGraph gr_x, gr_y; // 1 graph for each trajectory
      try {
        tmr.reset();
        prop.propagate( p, max_s );
        const float prop_time = tmr.elapsed()*1.e6;
        h_timing.Fill( prop_time*1.e-3 ); // in ms
      } catch ( Hector::ParticleStoppedException& e ) { }
      for ( const auto& pos : p ) {
        gr_x.SetPoint( gr_x.GetN(), pos.first, pos.second.position().x() );
        gr_y.SetPoint( gr_y.GetN(), pos.first, pos.second.position().y() );
      }
      gr_x.SetLineColor( kGray );
      gr_y.SetLineColor( kGray );
      gr_x.SetLineWidth( 1 );
      gr_y.SetLineWidth( 1 );
      mg_x[j].Add( (TGraph*)gr_x.Clone() );
      mg_y[j].Add( (TGraph*)gr_y.Clone() );
      ++j;
    }
  }

  // drawing part

  {
    ostringstream os_xa;
    for ( unsigned short i = 0; i < crossing_angles_x.size(); ++i ) {
      if ( i > 0 ) os_xa << "/";
      os_xa << Form( "%.1f", crossing_angles_x[i]*1.e6 );
    }
    Hector::Canvas c( "beamline", Form( "E_{p} = %.1f TeV, #alpha_{X} = %s #murad", Hector::Parameters::get()->beamEnergy()*CLHEP::GeV/CLHEP::TeV, os_xa.str().c_str() ), true );
    c.SetWindowSize( 800, 800 );
    //c.SetGrayscale();

    const bool draw_apertures = true;

    c.cd( 1 ); // x-axis
    if ( propagators.size() > 0 ) {
      auto mg = new TMultiGraph;
      for ( unsigned short j = 0; j < propagators.size(); ++j ) {
        TGraphErrors g_mean = mean_trajectory( mg_x[j] );
        g_mean.SetLineColor( ( j == 0 ) ? kBlack : kRed );
        g_mean.SetFillColorAlpha( ( j == 0 ) ? kBlack : kRed, 0.5 );
        //mg->Add( &mg_x[j] );
        mg->Add( (TGraph*)g_mean.Clone() );
      }
      mg->Draw( "a2" );
      mg->SetTitle( ".@@x (m)" );
      mg->GetXaxis()->SetLimits( 0., max_s );
      mg->GetYaxis()->SetRangeUser( -scale_x, scale_x );
      c.Prettify( mg->GetHistogram() );

      for ( unsigned short i = 0; i < propagators.size(); ++i )
        drawBeamline( 'x', propagators[i].beamline(), i, ip_name.c_str(), scale_x, 0., max_s, draw_apertures );

      mg->Draw( "l3" );
      mg->GetXaxis()->SetLabelSize( 0.055 );
      mg->GetYaxis()->SetLabelSize( 0.055 );
      mg->GetYaxis()->SetTitleSize( 0.1 );
      mg->GetYaxis()->SetTitleOffset( 0.65 );
      mg->GetXaxis()->SetTitle( "" );
    }
    c.GetPad( 1 )->SetGrid( 1, 1 );

    c.cd( 2 ); // y-axis
    if ( propagators.size() > 0 ) {
      auto mg = new TMultiGraph;
      for ( unsigned short j = 0; j < propagators.size(); ++j ) {
        TGraphErrors g_mean = mean_trajectory( mg_y[j] );
        g_mean.SetLineColor( ( j == 0 ) ? kBlack : kRed );
        g_mean.SetFillColorAlpha( ( j == 0 ) ? kBlack : kRed, 0.5 );
        //mg->Add( &mg_y[j] );
        mg->Add( (TGraph*)g_mean.Clone() );
      }
      mg->Draw( "a2" );
      mg->SetTitle( "s (m)@@y (m)" );
      mg->GetXaxis()->SetLimits( 0., max_s );
      mg->GetYaxis()->SetRangeUser( -scale_y, scale_y );
      c.Prettify( mg->GetHistogram() );

      for ( unsigned short i = 0; i < propagators.size(); ++i )
        drawBeamline( 'y', propagators[i].beamline(), i, ip_name.c_str(), scale_y, 0., max_s, draw_apertures );

      mg->Draw( "l3" );
      mg->GetXaxis()->SetLabelSize( 0.055 );
      mg->GetYaxis()->SetLabelSize( 0.055 );
      mg->GetXaxis()->SetTitleSize( 0.1 );
      mg->GetXaxis()->SetTitleOffset( 0.7 );
      mg->GetYaxis()->SetTitleSize( 0.1 );
      mg->GetYaxis()->SetTitleOffset( 0.65 );
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
    const char* label = Form( "#scale[0.33]{%s}", os.str().c_str() );
    auto pt = new Hector::Canvas::PaveText( 0.0, 0.0, 0.15, 0.01, label );
    pt->SetTextAlign( kHAlignLeft+kVAlignBottom );
    pt->Draw();

    c.Save( "pdf" );
  }
  { // draw the legend
    Hector::Canvas c( "beamline_legend", "", false, false );
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

TGraphErrors
mean_trajectory( const TMultiGraph& amg )
{
  map<double,double> mean_val; map<double,unsigned int> num_vals;
  // first loop to extract the mean and number of points
  for ( const auto& obj : *amg.GetListOfGraphs() ) {
    const auto gr = dynamic_cast<TGraph*>( obj );
    for ( int i = 0; i < gr->GetN(); ++i ) { double x, y; gr->GetPoint( i, x, y ); mean_val[x] += y; num_vals[x]++; }
  }

  for ( auto& v : mean_val ) v.second /= num_vals[v.first]; // extract the mean

  // second loop to extract the variance
  map<double,double> err_val;

  for ( const auto& obj : *amg.GetListOfGraphs() ) {
    const auto gr = dynamic_cast<TGraph*>( obj );
    for ( int i = 0; i < gr->GetN(); ++i ) { double x, y; gr->GetPoint( i, x, y ); err_val[x] += pow( y-mean_val[x], 2 )/num_vals[x]; }
  }

  for ( auto& v : err_val ) v.second = sqrt( v.second );
  assert( err_val.size() == mean_val.size() );

  //for ( const auto& v : num_vals ) cout << v.first << "|" << v.second << "|" << mean_val[v.first] << endl;
  TGraphErrors out;
  unsigned short i;
  i = 0; for ( const auto& v : mean_val ) { out.SetPoint( i, v.first, v.second ); i++; }
  i = 0; for ( const auto& v : err_val ) { out.SetPointError( i, 0., v.second ); i++; }
  return out;
}

