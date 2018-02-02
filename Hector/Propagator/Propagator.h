#ifndef Hector_Propagator_Propagator_h
#define Hector_Propagator_Propagator_h

#include "Hector/Propagator/Particle.h"

namespace Hector
{
  class Beamline;
  namespace Element {
    class ElementBase;
  }
  /// Main object to propagate particles through a beamline
  class Propagator
  {
    public:
      /// Construct the object for a given beamline
      Propagator( const Beamline* bl ) : beamline_( bl ) {}
      ~Propagator() {}

      const Beamline* beamline() const { return beamline_; }

      /// Propagate a particle up to a given position ; maps all state vectors to the intermediate s-coordinates
      void propagate( Particle&, float ) const;
      /// Check whether the particle has stopped inside a part of the beamline
      bool stopped( Particle&, float s_max=-1. ) const;

      /// Propagate a list of particle up to a given position ; maps all state vectors to the intermediate s-coordinates
      void propagate( Particles&, float s_max ) const;

    private:
      /// Extract a particle position at the exit of an element once it enters it
      Particle::Position propagateThrough( const Particle::Position& ini_pos, const std::shared_ptr<Element::ElementBase> ele, float eloss, int qp ) const;

      const Beamline* beamline_;
  };
}

#endif
