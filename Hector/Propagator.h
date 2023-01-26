/*
 *  Hector: a beamline propagation tool
 *  Copyright (C) 2016-2023  Laurent Forthomme
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef Hector_Propagator_h
#define Hector_Propagator_h

#include <memory>

#include "Hector/Elements/ElementFwd.h"
#include "Hector/Particle.h"

namespace hector {
  class Beamline;
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
                                        const element::ElementPtr& ele,
                                        double eloss,
                                        int qp) const;

    const Beamline* beamline_;  // NOT owning
  };
}  // namespace hector

#endif
