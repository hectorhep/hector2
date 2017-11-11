#include "Hector/Core/Exception.h"
#include "Hector/Core/Utils.h"
#include "Hector/IO/LHEHandler.h"

int
main( int argc, char* argv[] )
{
  if ( argc<2 ) {
    Hector::Exception( __PRETTY_FUNCTION__, Hector::Form( "Usage: %s [LHE input file]", argv[0] ), Hector::Fatal ).dump();
  }

  Hector::IO::LHE parser( argv[1] );

  parser.printInfo();

  Hector::Particles particles;
  while ( parser.nextEvent( particles ) ) {
    for ( const auto& part : particles ) {
      //it->dump();
      std::cout << "particle with pdgId=" << part.pdgId() << " has mass " << part.mass() << std::endl;
    }
  }

  return 0;
}
