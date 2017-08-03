#include "Hector/Core/Exception.h"
#include "Hector/Core/Utils.h"
#include "Hector/IO/LHEParser.h"

int
main( int argc, char* argv[] )
{
  if ( argc<2 ) {
    Hector::Exception( __PRETTY_FUNCTION__, Hector::Form( "Usage: %s [LHE input file]", argv[0] ), Hector::Fatal ).dump();
  }

  Hector::Parser::LHE parser( argv[1] );

  parser.printInfo();

  Hector::Particles part;
  while ( parser.nextEvent( part ) ) {
    for ( Hector::Particles::const_iterator it=part.begin(); it!=part.end(); it++ ) {
      //it->dump();
      std::cout << "particle with pdgId=" << it->pdgId() << " has mass " << it->mass() << std::endl;
    }
  }

  return 0;
}
