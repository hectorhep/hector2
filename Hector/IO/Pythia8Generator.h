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

#ifndef Hector_IO_Pythia8Generator_h
#define Hector_IO_Pythia8Generator_h

#include <Pythia8/Pythia.h>

#include <memory>
#include <string>
#include <vector>

#include "Hector/Particle.h"

namespace hector {
  /**
   * \brief Interface to the `Pythia8` generator
   * \author Laurent Forthomme
   * \date 2 May 2017
   */
  class Pythia8Generator {
  public:
    /// Create and configure a `Pythia8` instance
    Pythia8Generator(const std::vector<std::string>&);
    /// Create and configure a `Pythia8` instance
    /// \param[in] xml_input Input configuration file
    Pythia8Generator(const std::string& xml_input);
    ~Pythia8Generator();

    double crossSection() const {
      return pythia_->info.sigmaGen() * 1.e9;  // in pb
    }
    double crossSectionError() const { return pythia_->info.sigmaErr() * 1.e9; }
    /// Generate an event (list of output particles)
    /// \param[in] stable Only retrieve stable particles?
    Particles generate(bool stable = false);
    void generate(Particle& part);
    Particle diffractiveProton();

  private:
    std::unique_ptr<Pythia8::Pythia> pythia_;
  };
}  // namespace hector

#endif
