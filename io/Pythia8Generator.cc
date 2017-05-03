#include "Pythia8Generator.h"

namespace Hector
{
  Pythia8Generator::Pythia8Generator() :
    pythia_( std::make_unique<Pythia8::Pythia>() )
  {
    std::string config[] = {
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
      "MultipartonInteractions:expPow = 1.6"
    };
    try {
      // specify the incoming state
      pythia_->settings.mode( "Beams:idA", 2212 );
      pythia_->settings.mode( "Beams:idB", 2212 );
      pythia_->settings.parm( "Beams:eCM", 2.*Parameters::beam_energy ); //FIXME consider the asymmetric energies too!
      for ( unsigned short i=0; i<sizeof(config)/sizeof(config[0]); i++ ) {
        if ( !pythia_->readString( config[i].c_str() ) ) { throw Exception( __PRETTY_FUNCTION__, Form( "Failed to parse the command:\n\t  \"%s\"", config[i].c_str() ), JustWarning ); }
      }
      // initialise the single-diffraction
      pythia_->readString( "SoftQCD:singleDiffractive = on" );
      // disable the hadronisation
      pythia_->readString( "HadronLevel:all = off" );
      // initialise the core
      if ( !pythia_->init() ) { throw Exception( __PRETTY_FUNCTION__, "Failed to initialise the Pythia8 core", JustWarning ); }
    } catch ( Exception& e ) {
      e.dump();
    }
  }

  Pythia8Generator::Pythia8Generator( const char* xml_input ) :
    pythia_( std::make_unique<Pythia8::Pythia>( xml_input ) )
  {
    if ( !pythia_->init() ) { throw Exception( __PRETTY_FUNCTION__, "Failed to initialise the Pythia8 core", JustWarning ); }
  }

  Pythia8Generator::~Pythia8Generator()
  {
    pythia_->stat();
  }

  Particles
  Pythia8Generator::generate()
  {
    pythia_->next();
    Particles pout;

    const Pythia8::Event evt = pythia_->event;
    for ( unsigned short i=0; i<evt.size(); i++ ) {
      const Pythia8::Particle part = evt[i];
      if ( part.id()==9902210 ) { // diffractive proton
        pout.emplace_back( CLHEP::HepLorentzVector( part.px(), part.py(), part.pz(), part.e() ) );
      }
    }
    //evt.list();

    return pout;
  }
}
