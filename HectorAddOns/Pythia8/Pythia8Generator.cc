/*
 *  Hector: a beamline propagation tool
 *  Copyright (C) 2016-2023  Laurent Forthomme
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Pythia8/Event.h>

#include "Hector/Exception.h"
#include "Hector/Parameters.h"
#include "HectorAddOns/Pythia8/Pythia8Generator.h"

namespace hector {
  Pythia8Generator::Pythia8Generator(const std::vector<std::string>& params) : pythia_(new Pythia8::Pythia) {
    // specify the incoming state
    pythia_->settings.mode("Beams:idA", 2212);
    pythia_->settings.mode("Beams:idB", 2212);
    // one may also consider the asymmetric energies in the future
    pythia_->settings.parm("Beams:eCM", 2. * Parameters::get()->beamEnergy());
    // parameterise the generator
    for (const auto& param : params)
      if (!pythia_->readString(param.c_str()))
        throw H_ERROR << "Failed to parse the command:\n\t"
                      << "  \"" << param << "\"";
    // initialise the core
    if (!pythia_->init())
      throw H_ERROR << "Failed to initialise the Pythia8 core.";
  }

  Pythia8Generator::Pythia8Generator(const std::string& xml_input) : pythia_(new Pythia8::Pythia(xml_input)) {
    // initialise the core
    if (!pythia_->init())
      throw H_ERROR << "Failed to initialise the Pythia8 core.";
  }

  Pythia8Generator::~Pythia8Generator() {
    // dump some (useful?) generation statistics
    pythia_->stat();
  }

  Particles Pythia8Generator::generate(bool stable) {
    pythia_->next();
    const auto& evt = pythia_->event;

    evt.list(true, true);
    Particles pout;
    for (auto i = 0; i < evt.size(); ++i) {
      const auto& part = evt[i];
      if (!stable || part.status() > 0) {  // stable particles
        pout.emplace_back(LorentzVector(part.px(), part.py(), part.pz(), part.e()), (int)part.charge(), part.id());
      }
    }
    return pout;
  }

  Particle Pythia8Generator::diffractiveProton() {
    pythia_->next();

    const auto& evt = pythia_->event;
    for (auto i = 0; i < evt.size(); ++i) {
      const auto& part = evt[i];
      if (part.id() == 9902210) {  // diffractive proton
        return Particle(LorentzVector(part.px(), part.py(), part.pz(), part.e()), (int)part.charge(), part.id());
      }
    }
    throw H_ERROR << "Failed to generate the diffractive proton!";
  }

  void Pythia8Generator::generate(Particle& in_part) {
    pythia_->next();
    in_part.clear();

    const auto& evt = pythia_->event;
    for (auto i = 0; i < evt.size(); ++i) {
      const auto& part = evt[i];
      if (part.id() == 9902210) {  // diffractive proton
        in_part.firstStateVector().addMomentum(LorentzVector(part.px(), part.py(), part.pz(), part.e()));
        return;
      }
    }
    throw H_ERROR << "Failed to generate the modified kinematics for the particle template!";
  }
}  // namespace hector
