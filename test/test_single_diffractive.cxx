#include "Hector/IO/Pythia8Generator.h"
#include "Hector/IO/MADXHandler.h"
#include "Hector/Propagator/Propagator.h"

#include "Canvas.h"
#include "THStack.h"
#include "TH2.h"
#include "TH1.h"
#include "TStyle.h"

#include <CLHEP/Random/RandGauss.h>

using namespace std;

void plot_multi( const char* name, const char* title, vector<pair<string,TH1*> > graphs, const char* extra_label = "" );

int main( int argc, char* argv[] )
{
  if ( argc < 2 ) {
    cerr << "Usage: " << argv[0] << " <mad-x twiss file> [crossing angle (urad)]" << endl;
    return -1;
  }
  const double crossing_angle = ( argc > 2 ) ? atof( argv[2] ) : Hector::Parameters::get()->crossingAngleX();
  const double beam_divergence = 20.e-6; // in rad
  const double vertex_size = 10.e-6; // in m

  Hector::IO::MADX madx( argv[1], "IP5", 1, 250. );
  Hector::Propagator prop( madx.beamline() );
  const Hector::Elements rps = madx.romanPots( Hector::IO::MADX::horizontalPots );
  for ( const auto& rp : rps ) {
    cout << "--------> " << rp->name() << " at " << rp->s() << " m" << endl;
  }
  //const float s_pos = 220.;
  const float s_pos = (*rps.begin())->s();

  Hector::Parameters::get()->setComputeApertureAcceptance( false );
  // number of events to generate
  const unsigned short num_events = 5000;

  // configuration shamelessly stolen from CMSSW (9_1_X development cycle)
  vector<string> config{ {
    //"Next:numberCount = 0", // remove unnecessary output
    "Next:numberShowEvent = 0", // remove unnecessary output
    "Tune:preferLHAPDF = 2",
    "Main:timesAllowErrors = 10000",
    "Check:epTolErr = 0.01",
    "Beams:setProductionScalesFromLHEF = off",
    "SLHA:keepSM = on",
    "SLHA:minMassSM = 1000.",
    "ParticleDecays:limitTau0 = on",
    "ParticleDecays:tau0Max = 10",
    "ParticleDecays:allowPhotonRadiation = on",
    // CUETP8M1 tuning
    "Tune:pp 14",
    "Tune:ee 7",
    "MultipartonInteractions:pT0Ref = 2.4024",
    "MultipartonInteractions:ecmPow = 0.25208",
    "MultipartonInteractions:expPow = 1.6",
    // initialise the single-diffraction
    "SoftQCD:singleDiffractive = on",
    "SoftQCD:doubleDiffractive = off",
    // disable the hadronisation
    //"HadronLevel:all = off",
  } };
  Hector::Pythia8Generator gen( config );

  TH2D h_hitmap( "hitmap", "x (m)\\y (m)", 200, -0.015, 0.015, 200, -0.015, 0.015 );
  TH1D h_xi_raw( "xi_raw", "Proton momentum loss #xi\\Events\\?.3f", 250, -0.125, 1.125 ), h_xi_sp = *( ( TH1D* )h_xi_raw.Clone( "xi_scoring_plane" ) );
  TH1D h_tx_raw( "tx_raw", "#theta_{X}\\Events\\rad?.3f", 100, -0.5, 0.5 ), h_tx_sp = *( ( TH1D* )h_tx_raw.Clone( "tx_scoring_plane" ) );
  TH1D h_ty_raw( "ty_raw", "#theta_{Y}\\Events\\rad?.3f", 100, -0.5, 0.5 ), h_ty_sp = *( ( TH1D* )h_ty_raw.Clone( "ty_scoring_plane" ) );

  for ( unsigned short i = 0; i < num_events; ++i ) {
    const double ev_weight = 1./num_events;
    //auto part = gen.diffractiveProton();
    auto parts = gen.generate();
    for ( auto& part : parts ) {
      if ( part.pdgId() != 2212 ) continue;

      // smear the vertex and divergence ; apply the crossing angle
      auto ang = part.firstStateVector().angles(), pos = part.firstStateVector().position();
      ang[CLHEP::Hep2Vector::X] += CLHEP::RandGauss::shoot( crossing_angle, beam_divergence );
      pos[CLHEP::Hep2Vector::X] += CLHEP::RandGauss::shoot( 0., vertex_size );
      pos[CLHEP::Hep2Vector::Y] += CLHEP::RandGauss::shoot( 0., vertex_size );
      part.firstStateVector().setAngles( ang );
      part.firstStateVector().setPosition( pos );

      h_xi_raw.Fill( part.firstStateVector().xi(), ev_weight );
      h_tx_raw.Fill( part.firstStateVector().Tx(), ev_weight );
      h_ty_raw.Fill( part.firstStateVector().Ty(), ev_weight );

      // propagate to the pots position
      try {
        prop.propagate( part, s_pos );
        h_xi_sp.Fill( part.firstStateVector().xi(), ev_weight );
        h_tx_sp.Fill( part.firstStateVector().Tx(), ev_weight );
        h_ty_sp.Fill( part.firstStateVector().Ty(), ev_weight );
        const auto pos = part.stateVectorAt( s_pos ).position();
        h_hitmap.Fill( pos.x(), pos.y() );
      } catch ( Hector::Exception& e ) {}
    }
  }

  cout << "cross section: " << Form( "%.2e +/- %.2e pb", gen.crossSection(), gen.crossSectionError() ) << endl;

  gStyle->SetOptStat( 111111 );
  {
    const char* title = Form( "#alpha_{X} = %.2f #murad", crossing_angle*1.e6 );
    const char* sp_title = Form( "Protons in sc.plane (s = %.2f m)", s_pos );
    plot_multi( "xi_single_diffr", title, { { "Generated protons", &h_xi_raw }, { sp_title, &h_xi_sp, } }, argv[1] );
    plot_multi( "tx_single_diffr", title, { { "Generated protons", &h_tx_raw }, { sp_title, &h_tx_sp, } }, argv[1] );
    plot_multi( "ty_single_diffr", title, { { "Generated protons", &h_ty_raw }, { sp_title, &h_ty_sp, } }, argv[1] );
  }

  {
    Hector::Canvas c( "hitmap_single_diffr", "" );
    h_hitmap.Draw( "colz" );
    c.Prettify( &h_hitmap );
    c.Save( "pdf" );
  }

  return 0;
}

void
plot_multi( const char* name, const char* title, vector<pair<string,TH1*> > graphs, const char* extra_label )
{
  Hector::Canvas c( name, title );
  THStack st;
  c.SetLegendX1( 0.2 );
  unsigned short i = 0;
  for ( auto& gr : graphs ) {
    if ( i == 0 ) {
      st.SetTitle( gr.second->GetTitle() );
      gr.second->SetLineColor( kBlack );
    }
    else gr.second->SetLineColor( kBlack+i );
    c.AddLegendEntry( gr.second, gr.first.c_str(), ( i == 0 ) ? "l" : "lp" );
    gr.second->SetMarkerStyle( 23+i );
    st.Add( gr.second, ( i == 0 ) ? "hist" : "p" );
    ++i;
  }
  st.Draw( "nostack" );
  c.Prettify( st.GetHistogram() );
  st.SetTitle( "" );
  if ( strcmp( extra_label, "" ) != 0 ) {
    auto el = new Hector::Canvas::PaveText( 0.005, 0.05, 0.35, 0.05 );
    el->AddText( extra_label );
    el->SetTextSize( 0.02 );
    el->Draw();
  }
  c.SetLogy();
  c.Save( "pdf" );
}

