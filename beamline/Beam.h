#ifndef Beamline_Beam
#define Beamline_Beam

#include "Particle.h"

#include <vector>

class Beam
{
  public:
    Beam( const size_t& );
    ~Beam();

  private:
    std::vector<Particle> part_;
    float energy_;

};

#endif
