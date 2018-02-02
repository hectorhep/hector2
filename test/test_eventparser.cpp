#include "Hector/Core/Exception.h"
#include "Hector/Utils/Utils.h"
#include "Hector/Utils/ArgsParser.h"
#include "Hector/Utils/LHEHandler.h"

using namespace std;

int
main( int argc, char* argv[] )
{
  string lhe_file;
  Hector::ArgsParser( argc, argv, {
    { "lhe-file", "input LHE file", &lhe_file, 'i' },
  }, {} );

  Hector::IO::LHE parser( lhe_file );

  parser.printInfo();

  Hector::Particles particles;
  while ( parser.nextEvent( particles ) ) {
    for ( const auto& part : particles ) {
      //it->dump();
      cout << "particle with pdgId=" << part.pdgId() << " has mass " << part.mass() << endl;
    }
  }

  return 0;
}
