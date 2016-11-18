#include "beamline/Beamline.h"
#include "elements/Marker.h"

int
main( int argc, char* argv[] )
{
  Beamline line( 10. );
  Element::Marker marker( "test" );

  line.addElement( &marker );

  line.dump();

  //std::cout << "haha" << std::endl;

  return 0;
}
