#include "Pythia8Generator.h"

#include "Core/Parameters.h"
#include "Core/Utils.h"
#include "Core/Exception.h"

#include <Pythia8/Event.h>

namespace Hector
{
  Pythia8Generator::Pythia8Generator() :
    pythia_( std::make_unique<Pythia8::Pythia>() )
  {
    // parameterise the generator
    initialise();
    // initialise the core
    if ( !pythia_->init() ) {
      throw Exception( __PRETTY_FUNCTION__, "Failed to initialise the Pythia8 core", JustWarning );
    }
  }

  Pythia8Generator::Pythia8Generator( const char* xml_input ) :
    pythia_( std::make_unique<Pythia8::Pythia>( xml_input ) )
  {
    // initialise the core
    if ( !pythia_->init() ) {
      throw Exception( __PRETTY_FUNCTION__, "Failed to initialise the Pythia8 core", JustWarning );
    }
  }

  Pythia8Generator::~Pythia8Generator()
  {
    // dump some (useful?) generation statistics
    pythia_->stat();
  }

  void
  Pythia8Generator::initialise()
  {
    // configuration shamelessly stolen from CMSSW (9_1_X development cycle)
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
      pythia_->settings.parm( "Beams:eCM", 2.*Parameters::get()->beamEnergy() ); //FIXME consider the asymmetric energies too!
      for ( unsigned short i=0; i<sizeof(config)/sizeof(config[0]); i++ ) {
        if ( !pythia_->readString( config[i].c_str() ) ) { throw Exception( __PRETTY_FUNCTION__, Form( "Failed to parse the command:\n\t  \"%s\"", config[i].c_str() ), JustWarning ); }
      }
      // initialise the single-diffraction
      switchSingleDiffraction( true );
      switchDoubleDiffraction( false );
      // disable the hadronisation
      pythia_->readString( "HadronLevel:all = off" );
    } catch ( Exception& e ) {
      e.dump();
    }
  }

  Particles
  Pythia8Generator::generate()
  {
    pythia_->next();
    const Pythia8::Event evt = pythia_->event;

    Particles pout;
    for ( unsigned short i=0; i<evt.size(); i++ ) {
      const Pythia8::Particle part = evt[i];
      if ( part.id()==9902210 ) { // diffractive proton
        pout.emplace_back( CLHEP::HepLorentzVector( part.px(), part.py(), part.pz(), part.e() ), part.charge() );
      }
    }
    return pout;
  }

  Particle
  Pythia8Generator::diffractiveProton()
  {
    pythia_->next();

    const Pythia8::Event evt = pythia_->event;
    for ( unsigned short i=0; i<evt.size(); i++ ) {
      const Pythia8::Particle part = evt[i];
      if ( part.id()==9902210 ) { // diffractive proton
        return Particle( CLHEP::HepLorentzVector( part.px(), part.py(), part.pz(), part.e() ), part.charge() );
      }
    }
    throw Exception( __PRETTY_FUNCTION__, "Failed to generate the diffractive proton!", JustWarning );
  }

  void
  Pythia8Generator::generate( Particle& in_part )
  {
    pythia_->next();
    in_part.clear();

    const Pythia8::Event evt = pythia_->event;
    for ( unsigned short i=0; i<evt.size(); i++ ) {
      const Pythia8::Particle part = evt[i];
      if ( part.id()==9902210 ) { // diffractive proton
        in_part.firstStateVector().addMomentum( CLHEP::HepLorentzVector( part.px(), part.py(), part.pz(), part.e() ) );
        return;
      }
    }
    throw Exception( __PRETTY_FUNCTION__, "Failed to generate the modified kinematics for the particle template!", JustWarning );
  }
}
