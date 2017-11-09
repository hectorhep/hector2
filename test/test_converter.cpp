#include "Hector/IO/MADXHandler.h"
#include "Hector/IO/HBLFileHandler.h"

using namespace std;

int
main( int argc, char* argv[] )
{
  if ( argc < 2 ) {
    cerr << "Usage: " << argv[0] << " MadX_input_file" << endl;
    return -1;
  }

  Hector::IO::MADX in( argv[1], "IP5", +1 );
  Hector::IO::HBL::write( in.beamline(), "output.hbl" );

  Hector::IO::HBL reader( "output.hbl" );
  //reader.beamline()->dump();

  return 0;
}
