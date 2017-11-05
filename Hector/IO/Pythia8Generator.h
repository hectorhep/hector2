#ifndef Hector_IO_Pythia8Generator_h
#define Hector_IO_Pythia8Generator_h

#include "Propagator/Particle.h"

#include <Pythia8/Pythia.h>

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

      double crossSection() const { return pythia_->info.sigmaGen(); }
      double crossSectionError() const { return pythia_->info.sigmaErr(); }
      /// Generate an event (list of output particles)
      Particles generate();
      void generate( Particle& part );
      Particle diffractiveProton();

    private:
      void initialise();
      void switchSingleDiffraction( bool onoff ) { pythia_->settings.mode( "SoftQCD:singleDiffractive", onoff ); }
      void switchDoubleDiffraction( bool onoff ) { pythia_->settings.mode( "SoftQCD:doubleDiffractive", onoff ); }
      std::unique_ptr<Pythia8::Pythia> pythia_;
  };
}

#endif
