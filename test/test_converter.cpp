#include "Hector/IO/MADXHandler.h"
#include "Hector/Beamline/Beamline.h"
#include "Hector/IO/HBLFileHandler.h"
#include "Hector/Utils/ArgsParser.h"

using namespace std;

int
main( int argc, char* argv[] )
{
  string twiss_file, interaction_point, output_file;
  Hector::ArgsParser( argc, argv, {
    { "--twiss-file", "beamline Twiss file", &twiss_file },
  }, {
    { "--interaction-point", "name of the interaction point", "IP5", &interaction_point },
    { "--output", "output HBL file", "output.hbl", &output_file },
  } );

  Hector::IO::MADX in( twiss_file, interaction_point, +1 );
  Hector::IO::HBL::write( in.beamline(), output_file );

  Hector::IO::HBL reader( output_file );
  //reader.beamline()->dump();

  return 0;
}
