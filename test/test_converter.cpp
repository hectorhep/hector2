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

#include "Hector/Beamline.h"
#include "Hector/IO/HBLFileHandler.h"
#include "Hector/IO/TwissHandler.h"
#include "Hector/Utils/ArgsParser.h"

using namespace std;

int main(int argc, char* argv[]) {
  string twiss_file, interaction_point, output_file;
  double max_s;
  hector::ArgsParser(argc,
                     argv,
                     {
                         {"twiss-file", "beamline Twiss file", &twiss_file, 'i'},
                     },
                     {
                         {"ip-name", "name of the interaction point", "IP5", &interaction_point, 'c'},
                         {"max-s", "maximal s-coordinate (m)", 250., &max_s},
                         {"output", "output HBL file", "output.hbl", &output_file, 'o'},
                     });

  hector::io::Twiss in(twiss_file, interaction_point, max_s);
  hector::io::HBL::write(in.beamline(), output_file.c_str());

  hector::io::HBL reader(output_file);
  //reader.beamline()->dump();

  return 0;
}
