#include "Hector/Utils/ArgsParser.h"
#include "Hector/Utils/BeamProducer.h"
#include <iostream>

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
