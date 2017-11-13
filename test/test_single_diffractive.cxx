#include "Hector/Core/Exception.h"
#include "Hector/Beamline/Beamline.h"
#include "Hector/Utils/Pythia8Generator.h"
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
  const double crossing_angle = ( argc > 2 ) ? atof( argv[2] ) : 180.e-6;
  const double beam_divergence = 20.e-6; // in rad
  const double vertex_size = 10.e-6; // in m

  Hector::IO::MADX madx( argv[1], "IP5", 1, 250. );
  //madx.beamline()->offsetElementsAfter( 120., Hector::TwoVector( -0.097, 0. ) );

  Hector::Propagator prop( madx.beamline() );

  const Hector::Elements rps = madx.romanPots( Hector::IO::MADX::horizontalPots );
  double max_s = 0.;

  auto h_xi_raw = new TH1D( "xi_raw", "Proton momentum loss #xi\\Events\\?.3f", 250, -0.125, 1.125 ),
       h_tx_raw = new TH1D( "tx_raw", "#theta_{X}\\Events\\#murad?.1f", 100, -500., 500. ),
       h_ty_raw = new TH1D( "ty_raw", "#theta_{Y}\\Events\\#murad?.1f", 100, -500., 500. );
  map<Hector::Element::ElementBase*,TH1D*> h_xi_sp, h_tx_sp, h_ty_sp;
  map<Hector::Element::ElementBase*,TH2D*> h_hitmap;

  for ( const auto& pot : rps ) {
    const auto rp = pot.get();
    cout << "--------> " << rp->name() << " at " << rp->s() << " m" << endl;
    if ( rp->s() > max_s ) max_s = rp->s();
    h_xi_sp[rp] = ( TH1D* )h_xi_raw->Clone( Form( "xi_scoring_plane_%s", rp->name().c_str() ) );
    h_tx_sp[rp] = ( TH1D* )h_tx_raw->Clone( Form( "tx_scoring_plane_%s", rp->name().c_str() ) );
    h_ty_sp[rp] = ( TH1D* )h_ty_raw->Clone( Form( "ty_scoring_plane_%s", rp->name().c_str() ) );
    h_hitmap[rp] = new TH2D( Form( "hitmap_%s", rp->name().c_str() ), "x (m)\\y (m)", 300, -0.15, 0., 300, -0.03, 0.03 );
  }

  Hector::Parameters::get()->setComputeApertureAcceptance( false );
  // number of events to generate
  const unsigned short num_events = 2500;

  // configuration shamelessly stolen from CMSSW (9_1_X development cycle)
  vector<string> config{ {
    "Next:numberCount = 5000", // remove unnecessary output
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

  for ( unsigned short i = 0; i < num_events; ++i ) {
    const double ev_weight = 1./num_events;
    //auto part = gen.diffractiveProton();
    auto parts = gen.generate();
    for ( auto& part : parts ) {
      if ( part.pdgId() != 2212 ) continue;

      // smear the vertex and divergence ; apply the crossing angle
      auto ang = part.firstStateVector().angles(), pos = part.firstStateVector().position();
      ang[CLHEP::Hep2Vector::X] += CLHEP::RandGauss::shoot( crossing_angle, beam_divergence );
      ang[CLHEP::Hep2Vector::Y] += CLHEP::RandGauss::shoot( 0., beam_divergence );
      pos[CLHEP::Hep2Vector::X] += CLHEP::RandGauss::shoot( 0., vertex_size );
      pos[CLHEP::Hep2Vector::Y] += CLHEP::RandGauss::shoot( 0., vertex_size );
      part.firstStateVector().setAngles( ang );
      part.firstStateVector().setPosition( pos );

      h_xi_raw->Fill( part.firstStateVector().xi(), ev_weight );
      h_tx_raw->Fill( part.firstStateVector().Tx()*1.e6, ev_weight );
      h_ty_raw->Fill( part.firstStateVector().Ty()*1.e6, ev_weight );

      // propagate to the pots position
      try {
        prop.propagate( part, max_s );
        for ( const auto& pot : rps ) {
          const auto rp = pot.get();
          const auto pos = part.stateVectorAt( rp->s() ).position();
          h_hitmap[rp]->Fill( pos.x(), pos.y() );
          h_xi_sp[rp]->Fill( part.firstStateVector().xi(), ev_weight );
          h_tx_sp[rp]->Fill( part.firstStateVector().Tx()*1.e6, ev_weight );
          h_ty_sp[rp]->Fill( part.firstStateVector().Ty()*1.e6, ev_weight );
        }
      } catch ( Hector::Exception& e ) {
        //e.dump();
      }
    }
  }

  cout << "cross section: " << Form( "%.2e +/- %.2e pb", gen.crossSection(), gen.crossSectionError() ) << endl;

  gStyle->SetOptStat( 111111 );
  {
    const char* title = Form( "#alpha_{X} = %.2f #murad", crossing_angle*1.e6 );
    vector<pair<string,TH1*> > gr_xi = { { "Generated protons", h_xi_raw } },
                               gr_tx = { { "Generated protons", h_tx_raw } },
                               gr_ty = { { "Generated protons", h_ty_raw } };
    for ( const auto& pot : rps ) {
      const auto& rp = pot.get();
      gr_xi.emplace_back( Form( "Protons in %s", rp->name().c_str() ), h_xi_sp[rp] );
      gr_tx.emplace_back( Form( "Protons in %s", rp->name().c_str() ), h_tx_sp[rp] );
      gr_ty.emplace_back( Form( "Protons in %s", rp->name().c_str() ), h_ty_sp[rp] );
    }
    const string bottom_label = Form( "%s - N = %d protons", argv[1], num_events );
    plot_multi( "xi_single_diffr", title, gr_xi, bottom_label.c_str() );
    plot_multi( "tx_single_diffr", title, gr_tx, bottom_label.c_str() );
    plot_multi( "ty_single_diffr", title, gr_ty, bottom_label.c_str() );
  }

  for ( const auto& pot : rps ) {
    const auto& rp = pot.get();
    Hector::Canvas c( Form( "hitmap_single_diffr_%s", rp->name().c_str() ), "" );
    h_hitmap[rp]->Draw( "colz" );
    c.Prettify( h_hitmap[rp] );
    c.Save( "pdf" );
  }

  return 0;
}

void
plot_multi( const char* name, const char* title, vector<pair<string,TH1*> > graphs, const char* extra_label )
{
  Hector::Canvas c( name, title );
  THStack st;
  c.SetLegendX1( 0.225 );
  unsigned short i = 0;
  for ( auto& gr : graphs ) {
    if ( i == 0 ) {
      st.SetTitle( gr.second->GetTitle() );
      gr.second->SetLineColor( kBlack );
      gr.second->Fit( "gaus", "+" );
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

