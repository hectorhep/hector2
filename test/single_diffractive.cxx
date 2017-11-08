#include "Hector/IO/Pythia8Generator.h"
#include "Hector/IO/MADXParser.h"
#include "Hector/Propagator/Propagator.h"

#include "Canvas.h"
#include "TH2.h"
#include "TH1.h"

int main( int argc, char* argv[] )
{
  if ( argc<2 ) {
    std::cerr << "Usage: " << argv[0] << " <mad-x twiss file>" << std::endl;
    return -1;
  }
  Hector::Parser::MADX madx( argv[1], "IP5", 1, 250. );
  Hector::Propagator prop( madx.beamline() );
  const Hector::Elements rps = madx.romanPots( Hector::Parser::MADX::horizontalPots );
  for ( const auto& rp : rps ) {
    std::cout << "--------> " << rp->name() << " at " << rp->s() << std::endl;
  }
  const float s_pos = 220.;

  Hector::Parameters::get()->setComputeApertureAcceptance( false );
  // number of events to generate
  const unsigned short num_events = 10000;

  // configuration shamelessly stolen from CMSSW (9_1_X development cycle)
  std::vector<std::string> config{ {
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
    "HadronLevel:all = off",
  } };
  Hector::Pythia8Generator gen( config );

  TH2D h_hitmap( "hitmap", "x (m)\\y (m)", 200, -0.015, 0.015, 200, -0.015, 0.015 );
  TH1D h_xidiff( "xi", "#xi\\Events\\?.3f", 200, -0.5, 0.5 );

  for ( unsigned short i=0; i<num_events; i++ ) {
    Hector::Particle part = gen.diffractiveProton();
    try {
      prop.propagate( part, s_pos );
      h_xidiff.Fill( part.firstStateVector().xi() );
      const CLHEP::Hep2Vector pos = part.stateVectorAt( s_pos ).position();
      h_hitmap.Fill( pos.x(), pos.y() );
    } catch ( Hector::Exception& e ) {}
  }

  std::cout << "cross section: " << gen.crossSection() << " +/- " << gen.crossSectionError() << std::endl;

  {
    Hector::Canvas c( "xi_single_diffr", "" );
    h_xidiff.Draw();
    c.Prettify( &h_xidiff );
    c.SetLogy();
    c.Save( "pdf" );
  }

  {
    Hector::Canvas c( "hitmap_single_diffr", "" );
    h_hitmap.Draw( "colz" );
    c.Prettify( &h_hitmap );
    c.Save( "pdf" );
  }

  return 0;
}
