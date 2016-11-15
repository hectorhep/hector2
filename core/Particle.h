#ifndef core_Particle_h
#define core_Particle_h

#include <LorentzVector.h>

class Particle
{
  public:
   Particle();
   ~Particle();

  private:
    CLHEP::HepLorentzVector mom_;
};

#endif
