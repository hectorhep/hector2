#include "Pythia8Generator.h"

namespace Hector
{
  Pythia8Generator::Pythia8Generator()
  {
    pythia_ = std::make_unique<Pythia8::Pythia>();
    pythia_->readString( "SoftQCD:singleDiffractive = on" );
    // disable the hadronisation
    //Pythia8::Pythia::readString( "HadronLevel:all = off" );
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
      "MultipartonInteractions:pT0Ref=2.4024",
      "MultipartonInteractions:ecmPow=0.25208",
      "MultipartonInteractions:expPow=1.6"
    };
    for ( unsigned short i=0; i<sizeof(config)/sizeof(config[0]); i++ ) {
      pythia_->readString( config[i].c_str() );
    }
     pythia_->settings.mode( "Beams::idA", 2212 ); //FIXME consider the asymmetric energies too!
     pythia_->settings.mode( "Beams::idB", 2212 );
     pythia_->settings.mode( "Beams::eCM", 2.*Parameters::beam_energy );

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
    evt.list();

    return pout;
  }
}
