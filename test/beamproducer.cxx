#include "propagator/BeamProducer.h"

#include "Canvas.h"

/// \test Produce a vector of particles for a given pattern
int
main( int argc, char* argv[] )
{
  Hector::BeamProducer::gaussianParticleGun gun( 6000., 6500. );
  for ( unsigned short i=0; i<10; i++ ) {
    Hector::Particle p = gun.shoot();
    //std::cout << "--> " << p->firstStateVector().energy() << " ---> " << p->firstStateVector().m() << " | " << p->firstStateVector().momentum() << std::endl;
    p.dump();
  }

  return 0;
}
