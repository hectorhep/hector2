#include "Hector/Beamline/Beamline.h"
#include "Hector/Elements/Marker.h"

int
main()
{
  Hector::Beamline line( 10. );
  
  /*Hector::Element::Marker marker( "test" );
  line.add( &marker );*/
  line.add( std::make_shared<Hector::Element::Marker>( "test", 0., 0. ) );

  line.dump();

  return 0;
}
