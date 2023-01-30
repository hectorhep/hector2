/*
 *  Hector: a beamline propagation tool
 *  Copyright (C) 2016-2023  Laurent Forthomme
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>

#include "Hector/Beamline.h"
#include "Hector/Elements/Marker.h"

int main() {
  hector::Beamline line(10.);

  /*hector::Element::Marker marker( "test" );
  line.add( &marker );*/
  line.add(std::make_shared<hector::element::Marker>("test", 0., 0.));

  line.dump(std::cout);

  return 0;
}
