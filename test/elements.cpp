#include "Hector/Beamline.h"
#include "Hector/Elements/Marker.h"
#include <iostream>

int main() {
  hector::Beamline line(10.);

  /*hector::Element::Marker marker( "test" );
  line.add( &marker );*/
  line.add(std::make_shared<hector::element::Marker>("test", 0., 0.));

  line.dump(std::cout);

  return 0;
}
