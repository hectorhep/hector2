#include "io/MADXParser.h"
#include "core/Particle.h"

int main( int argc, char* argv[] )
{
  Parser::MADX( "data/twiss/twiss_coll0p4m_ir5b1_6p5tev.tfs", +1, 250. );

  return 0;
}
