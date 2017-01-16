#include "core/Exception.h"
#include "core/Utils.h"
#include "io/LHEParser.h"

int
main( int argc, char* argv[] )
{
  if ( argc<2 ) {
    Hector::Exception( __PRETTY_FUNCTION__, Hector::Form( "Usage: %s [LHE input file]", argv[0] ), Hector::Fatal ).dump();
  }

  Hector::Parser::LHE parser( argv[1] );

  parser.printInfo();
  Hector::Particles part = parser.nextEvent();
  for ( Hector::Particles::const_iterator it=part.begin(); it!=part.end(); it++ ) {
    it->dump();
  }

  return 0;
}
