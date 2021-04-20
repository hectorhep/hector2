#include "Hector/IO/TwissHandler.h"
#include "Hector/Beamline/Beamline.h"
#include "Hector/IO/HBLFileHandler.h"
#include "Hector/Utils/ArgsParser.h"

using namespace std;

int main(int argc, char* argv[]) {
  string twiss_file, interaction_point, output_file;
  double max_s;
  Hector::ArgsParser(argc,
                     argv,
                     {
                         {"twiss-file", "beamline Twiss file", &twiss_file, 'i'},
                     },
                     {
                         {"ip-name", "name of the interaction point", "IP5", &interaction_point, 'c'},
                         {"max-s", "maximal s-coordinate (m)", 250., &max_s},
                         {"output", "output HBL file", "output.hbl", &output_file, 'o'},
                     });

  Hector::IO::Twiss in(twiss_file, interaction_point, max_s);
  Hector::IO::HBL::write(in.beamline(), output_file.c_str());

  Hector::IO::HBL reader(output_file);
  //reader.beamline()->dump();

  return 0;
}
