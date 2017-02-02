#ifndef Propagator_Propagator_h
#define Propagator_Propagator_h

#include "beamline/Beamline.h"
#include "BeamProducer.h"
#include "Particle.h"

namespace Hector
{
  /// Main object to propagate particles through a beamline
  class Propagator
  {
    public:
      /// Construct the object for a given beamline
      Propagator( const Beamline* );
      ~Propagator();

      /// Propagate a particle up to a given position ; maps all state vectors to the intermediate s-coordinates
      void propagate( Particle&, float ) const;
      /// Get the s-coordinate/state vector for the particle at the level of a beamline element
      Particle::Position hitPosition( const Particle::StateVector& ini_pos, const Element::ElementBase* ele, float eloss, float mp, int qp ) const;

      /// Propagate a list particle up to a given position ; maps all state vectors to the intermediate s-coordinates
      void propagate( Particles& ) const;

    private:
      /// Extract a particle position at the exit of an element once it enters it
      Particle::Position propagateThrough( const Particle::Position& ini_pos, const Element::ElementBase* ele, float eloss, int qp ) const;

      const Beamline* beamline_;
  };
}

#endif
