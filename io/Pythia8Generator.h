#ifndef Hector_IO_Pythia8Generator_h
#define Hector_IO_Pythia8Generator_h

#include "core/Parameters.h"
#include "core/Utils.h"
#include "propagator/Particle.h"

#include <Pythia8/Pythia.h>
#include <Pythia8/Event.h>

#include <memory>

namespace Hector
{
  class Pythia8Generator
  {
    public:
      Pythia8Generator();
      ~Pythia8Generator();

      Particles generate();

    private:
      std::unique_ptr<Pythia8::Pythia> pythia_;
  };
}

#endif
