#include "Hector/Utils/Pythia8Generator.h"
#include "Hector/Utils/Utils.h"

#include "Hector/Core/Parameters.h"
#include "Hector/Core/Exception.h"

#include <Pythia8/Event.h>

namespace hector {
  Pythia8Generator::Pythia8Generator(const std::vector<std::string>& params) : pythia_(new Pythia8::Pythia) {
    // specify the incoming state
    pythia_->settings.mode("Beams:idA", 2212);
    pythia_->settings.mode("Beams:idB", 2212);
    pythia_->settings.parm("Beams:eCM",
                           2. * Parameters::get()->beamEnergy());  //FIXME consider the asymmetric energies too!
    // parameterise the generator
    for (const auto& param : params)
      if (!pythia_->readString(param.c_str()))
        throw Exception(__PRETTY_FUNCTION__, JustWarning) << "Failed to parse the command:\n\t  \"" << param << "\"";
    // initialise the core
    if (!pythia_->init())
      throw Exception(__PRETTY_FUNCTION__, JustWarning) << "Failed to initialise the Pythia8 core.";
  }

  Pythia8Generator::Pythia8Generator(const char* xml_input) : pythia_(new Pythia8::Pythia(xml_input)) {
    // initialise the core
    if (!pythia_->init())
      throw Exception(__PRETTY_FUNCTION__, JustWarning) << "Failed to initialise the Pythia8 core.";
  }

  Pythia8Generator::~Pythia8Generator() {
    // dump some (useful?) generation statistics
    pythia_->stat();
  }

  Particles Pythia8Generator::generate(bool stable) {
    pythia_->next();
    const Pythia8::Event evt = pythia_->event;

    evt.list(true, true);
    Particles pout;
    for (int i = 0; i < evt.size(); ++i) {
      const Pythia8::Particle part = evt[i];
      if (!stable || part.status() > 0) {  // stable particles
        pout.emplace_back(LorentzVector(part.px(), part.py(), part.pz(), part.e()), (int)part.charge(), part.id());
      }
    }
    return pout;
  }

  Particle Pythia8Generator::diffractiveProton() {
    pythia_->next();

    const Pythia8::Event evt = pythia_->event;
    for (int i = 0; i < evt.size(); ++i) {
      const Pythia8::Particle part = evt[i];
      if (part.id() == 9902210) {  // diffractive proton
        return Particle(LorentzVector(part.px(), part.py(), part.pz(), part.e()), (int)part.charge(), part.id());
      }
    }
    throw Exception(__PRETTY_FUNCTION__, JustWarning) << "Failed to generate the diffractive proton!";
  }

  void Pythia8Generator::generate(Particle& in_part) {
    pythia_->next();
    in_part.clear();

    const Pythia8::Event evt = pythia_->event;
    for (int i = 0; i < evt.size(); ++i) {
      const Pythia8::Particle part = evt[i];
      if (part.id() == 9902210) {  // diffractive proton
        in_part.firstStateVector().addMomentum(LorentzVector(part.px(), part.py(), part.pz(), part.e()));
        return;
      }
    }
    throw Exception(__PRETTY_FUNCTION__, JustWarning)
        << "Failed to generate the modified kinematics for the particle template!";
  }
}  // namespace hector
