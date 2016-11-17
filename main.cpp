#include "io/MADXParser.h"

int main( int argc, char* argv[] )
{
  Parser::MADX parser( "data/twiss/twiss_coll0p4m_ir5b1_6p5tev.tfs", +1 );

  std::cout << "beamline matrix: " << parser.beamline().matrix( 100., 0.938, +1 ) << std::endl;

  return 0;
}
