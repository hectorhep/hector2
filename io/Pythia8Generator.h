#ifndef Hector_IO_Pythia8Generator_h
#define Hector_IO_Pythia8Generator_h

#include "core/Parameters.h"
#include "core/Utils.h"
#include "propagator/Particle.h"

#include <Pythia8/Pythia.h>
#include <Pythia8/Event.h>

namespace Hector
{
  class Pythia8Generator : public Pythia8::Pythia
  {
    public:
      Pythia8Generator();
      ~Pythia8Generator();

      Particles generate();

    private:
  };
}

#endif
