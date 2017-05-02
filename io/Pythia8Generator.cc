#include "Pythia8Generator.h"

namespace Hector
{
  Pythia8Generator::Pythia8Generator() :
    Pythia8::Pythia()
  {
    Pythia8::Pythia::readString( "SoftQCD:singleDiffractive = on" );
    // disable the hadronisation
    //Pythia8::Pythia::readString( "HadronLevel:all = off" );
    std::string config[] = {
      Form( "Beams::idA = %d", 2212 ), //FIXME consider the asymmetric energies too!
      Form( "Beams::idB = %d", 2212 ),
      Form( "Beams::eCM = %f", 2.*Parameters::beam_energy ),
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
      Pythia8::Pythia::readString( config[i].c_str() );
    }
  }

  Pythia8Generator::~Pythia8Generator()
  {
    Pythia8::Pythia::stat();
  }

  Particles
  Pythia8Generator::generate()
  {
    Pythia8::Pythia::next();
    Particles pout;

    const Pythia8::Event evt = Pythia8::Pythia::event;
    evt.list();

    return pout;
  }
}
