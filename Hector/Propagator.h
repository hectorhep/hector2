#ifndef Hector_Propagator_h
#define Hector_Propagator_h

#include "Hector/Particle.h"
#include <memory>

namespace hector {
  class Beamline;
  namespace element {
    class ElementBase;
  }
  /// Main object to propagate particles through a beamline
  class Propagator {
  public:
    /// Construct the object for a given beamline
    Propagator(const Beamline* bl) : beamline_(bl) {}
    ~Propagator() {}

    const Beamline* beamline() const { return beamline_; }

    /// Propagate a particle up to a given position ; maps all state vectors to the intermediate s-coordinates
    void propagate(Particle&, double) const;
    /// Check whether the particle has stopped inside a part of the beamline
    bool stopped(Particle&, double s_max = -1.) const;

    /// Propagate a list of particle up to a given position ; maps all state vectors to the intermediate s-coordinates
    void propagate(Particles&, double s_max) const;

  private:
    /// Extract a particle position at the exit of an element once it enters it
    Particle::Position propagateThrough(const Particle::Position& ini_pos,
                                        const std::shared_ptr<element::ElementBase> ele,
                                        double eloss,
                                        int qp) const;

    const Beamline* beamline_;  // NOT owning
  };
}  // namespace hector

#endif
