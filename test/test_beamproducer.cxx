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

#include "Hector/Utils/ArgsParser.h"
#include "Hector/Utils/BeamProducer.h"

using namespace std;

/// \test Produce a vector of particles for a given pattern
int main(int argc, char* argv[]) {
  unsigned int num_particles;
  double min_xi, max_xi;
  hector::ArgsParser(argc,
                     argv,
                     {},
                     {
                         {"num-parts", "number of particles to generate", 100, &num_particles, 'n'},
                         {"min-xi", "minimal particle momentum loss", 0., &min_xi},
                         {"max-xi", "maximal particle momentum loss", 0.1, &max_xi},
                     });

  //hector::beam::gaussianParticleGun gun( 6000., 6500. );
  hector::beam::Xiscanner gun(num_particles, min_xi, max_xi);
  for (unsigned short i = 0; i < num_particles; ++i) {
    hector::Particle p = gun.shoot();
    //p.dump();
    cout << p.firstStateVector().xi() << endl;
  }

  return 0;
}
