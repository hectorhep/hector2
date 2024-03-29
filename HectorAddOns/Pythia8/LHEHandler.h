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

#ifndef Hector_IO_LHEHandler_h
#define Hector_IO_LHEHandler_h

#ifdef HEPMC
#include "HepMC/Version.h"
#ifdef HEPMC_VERSION_CODE  // HepMC v3+
#define GOOD_HEPMC
#include "HepMC/LHEF.h"
#endif
#endif
#ifndef GOOD_HEPMC
#ifdef PYTHIA8
#include <Pythia8/Pythia.h>
#endif
#endif

#include "Hector/Particle.h"

namespace hector {
  namespace io {
    /// Parsing tool for LHEF files to use as beams of primary particles
    class LHE {
    public:
      /// Parse a LHEF file to build a particles vector for each event
      /// \param[in] filename Path to the .lhe file to parse
      LHE(const std::string& filename);
      ~LHE();

      /// Display general information retrieved from the LHEF file
      void printInfo() const;

      /// Retrieve the next event in the LHEF bank
      /// \return True, if the event retrieved is valid, false otherwise
      /// \param[out] parts Vector of final state particles contained in the event
      bool nextEvent(Particles& parts);

      /// Process cross section (in pb)
      float crossSection() const;
      /// Error on the process cross section (in pb)
      float crossSectionError() const;

      /// PDG id of the positive z beam
      int beam1PDGId() const;
      /// PDG id of the negative z beam
      int beam2PDGid() const;
      /// Energy of the positive z beam (in GeV)
      float beam1Energy() const;
      /// Energy of the negative z beam (in GeV)
      float beam2Energy() const;

    private:
#ifdef GOOD_HEPMC
      std::unique_ptr<LHEF::Reader> reader_;
#else
#ifdef PYTHIA8
      std::unique_ptr<Pythia8::Pythia> pythia_;
#endif
#endif
    };
  }  // namespace io
}  // namespace hector

#endif
