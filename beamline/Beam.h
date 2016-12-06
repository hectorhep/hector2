#ifndef Beamline_Beam
#define Beamline_Beam

#include "propagator/Particle.h"

#include <vector>

namespace Hector
{
  /// Set of particles composing a beam
  class Beam
  {
    public:
      typedef std::vector<Particle> Particles;

    public:
      /// Build a beam given a particles multiplicity
      Beam( const size_t& );
      ~Beam();
      
      /// Constant iterator to the first particle in the beam
      const Particles::const_iterator particles_begin() const { return part_.begin(); }
      /// Constant iterator to the last particle in the beam
      const Particles::const_iterator particles_end() const { return part_.end(); }

    private:
      Particles part_;
      float energy_;

  };
}

#endif
