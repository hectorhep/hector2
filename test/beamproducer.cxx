#include "Hector/Propagator/BeamProducer.h"

/// \test Produce a vector of particles for a given pattern
int
main( int argc, char* argv[] )
{
  const unsigned short num_particles = 101;
  //Hector::BeamProducer::gaussianParticleGun gun( 6000., 6500. );
  Hector::BeamProducer::Xiscanner gun( num_particles, 0., 0.1 );
  for ( unsigned short i=0; i<num_particles; i++ ) {
    Hector::Particle p = gun.shoot();
    //p.dump();
    std::cout << p.firstStateVector().xi() << std::endl;
  }

  return 0;
}
