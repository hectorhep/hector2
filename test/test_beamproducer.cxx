#include "Hector/Utils/ArgsParser.h"
#include "Hector/Utils/BeamProducer.h"

using namespace std;

/// \test Produce a vector of particles for a given pattern
int main(int argc, char* argv[]) {
  unsigned int num_particles;
  double min_xi, max_xi;
  Hector::ArgsParser(argc,
                     argv,
                     {},
                     {
                         {"num-parts", "number of particles to generate", 100, &num_particles, 'n'},
                         {"min-xi", "minimal particle momentum loss", 0., &min_xi},
                         {"max-xi", "maximal particle momentum loss", 0.1, &max_xi},
                     });

  //Hector::BeamProducer::gaussianParticleGun gun( 6000., 6500. );
  Hector::BeamProducer::Xiscanner gun(num_particles, min_xi, max_xi);
  for (unsigned short i = 0; i < num_particles; ++i) {
    Hector::Particle p = gun.shoot();
    //p.dump();
    cout << p.firstStateVector().xi() << endl;
  }

  return 0;
}
