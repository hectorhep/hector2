#ifndef core_Particle_h
#define core_Particle_h

#include <CLHEP/Vector/LorentzVector.h>

class Particle
{
  public:
   Particle();
   ~Particle();

  private:
    CLHEP::HepLorentzVector mom_;
    CLHEP::HepLorentzVector pos_;
    float charge_;
};

#endif
