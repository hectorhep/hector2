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

#include "Hector/Exception.h"
#include "Hector/Utils/ArgsParser.h"
#include "HectorAddOns/Pythia8/LHEHandler.h"

using namespace std;

int main(int argc, char* argv[]) {
  string lhe_file;
  hector::ArgsParser(argc,
                     argv,
                     {
                         {"lhe-file", "input LHE file", &lhe_file, 'i'},
                     },
                     {});

  hector::io::LHE parser(lhe_file.c_str());

  parser.printInfo();

  hector::Particles particles;
  while (parser.nextEvent(particles)) {
    for (const auto& part : particles) {
      //it->dump();
      H_INFO << "particle with pdgId=" << part.pdgId() << " has mass " << part.mass() << ".";
    }
  }

  return 0;
}
