#include "io/MadXparser.h"
#include "core/Particle.h"

int main( int argc, char* argv[] )
{
  Parser::MadX( "data/twiss/twiss_coll0p4m_ir5b1_6p5tev.tfs", +1 );

  return 0;
}
