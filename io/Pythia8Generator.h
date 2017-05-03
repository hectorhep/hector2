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
  /**
   * \brief Interface to the `Pythia8` generator
   * \author Laurent Forthomme
   * \date 2 May 2017
   */
  class Pythia8Generator
  {
    public:
      /// Create and configure a `Pythia8` instance
      Pythia8Generator();
      /// Create and configure a `Pythia8` instance
      /// \param[in] xml_input Input configuration file
      Pythia8Generator( const char* xml_input );
      ~Pythia8Generator();

      /// Generate an event (list of output particles)
      Particles generate();

    private:
      std::unique_ptr<Pythia8::Pythia> pythia_;
  };
}

#endif
