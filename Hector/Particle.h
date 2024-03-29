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

#ifndef Hector_Particle_h
#define Hector_Particle_h

#include <iosfwd>
#include <map>
#include <vector>

#include "Hector/Utils/StateVector.h"

namespace hector {
  /// Generic particle model inserted in a beam
  class Particle {
  public:
    /// Particle trajectory holder ; map of state vectors indexed to the s-position
    typedef std::map<double, StateVector> PositionsMap;
    /// Pair of s-position and state vector defining the particle kinematics
    class Position : private std::pair<double, StateVector> {
    public:
      /// Construct from a positions map iterator
      Position(PositionsMap::const_iterator it) : std::pair<double, StateVector>(*it) {}
      /// Construct from a pair s-state vector
      Position(std::pair<double, StateVector> pair) : std::pair<double, StateVector>(pair) {}
      /// Construct a couple between a longitudinal position and a state vector
      Position(double s, const StateVector& sv) : std::pair<double, StateVector>(s, sv) {}
      /// Longitudinal position
      double s() const { return this->first; }
      /// State vector components
      StateVector& stateVector() { return this->second; }
      /// State vector components
      const StateVector stateVector() const { return this->second; }
      /// A pair of longitudinal position/state vector
      const std::pair<double, StateVector>& pair() const { return *this; }
    };

  public:
    Particle();
    /// Construct a particle according to its first state vector/s-position couple
    /// \param[in] sv0 State vector at initial position \a s0
    /// \param[in] s0 Longitudinal initial position (in m)
    Particle(const StateVector& sv0, double s0 = 0.);
    /// Construct a particle according to its first state vector's 4-momentum
    /// \param[in] mom Initial 4-momentum
    /// \param[in] charge Electric charge (in units of e)
    /// \param[in] pdgid PDG id
    Particle(const LorentzVector& mom, int charge = 999, int pdgid = 2212);
    ~Particle();

    /// Build a Particle object from a mass and electric charge
    static Particle fromMassCharge(double mass, int charge);

    /// Clear all state vectors (but the initial one)
    void clear() { positions_.erase(++begin(), end()); }
    /// Add a new s-position/state vector couple to the particle's trajectory
    /// \param[in] stopped Has the particle been stopped in the process?
    void addPosition(double s, const StateVector& vec, bool stopped = false) { addPosition(Position(s, vec), stopped); }
    /// Add a new position (s-position/state vector couple) to the particle's trajectory
    /// \param[in] stopped Has the particle been stopped in the process?
    void addPosition(const Position& pos, bool stopped = false);

    /// Set the electric charge (in units of e)
    void setCharge(int ch) { charge_ = ch; }
    /// Electric charge (in units of e)
    int charge() const { return charge_; }

    /// Set the particle's PDG id
    void setPDGid(int pdgid) { pdgId_ = pdgid; }
    /// Particle's PDG id
    int pdgId() const { return pdgId_; }

    /// Mass (in GeV/c2)
    double mass() const { return firstStateVector().m(); }

    /// Set if the particle has physical kinematics
    void setPhysical(bool phys) { physical_ = phys; }
    /// Reasonable kinematics for the particle?
    bool physical() const { return physical_; }

    /// Print all useful information about a particle
    void dump(std::ostream& os) const;

    /// First position associated to the particle along s
    double firstS() const { return positions_.begin()->first; }
    /// Last position associated to the particle along s
    double lastS() const { return positions_.rbegin()->first; }

    /// First state vector associated to the particle
    StateVector& firstStateVector() { return positions_.begin()->second; }
    /// Last state vector associated to the particle
    StateVector& lastStateVector() { return positions_.rbegin()->second; }
    /// First state vector associated to the particle
    const StateVector firstStateVector() const { return positions_.begin()->second; }
    /// Last state vector associated to the particle
    const StateVector lastStateVector() const { return positions_.rbegin()->second; }

    PositionsMap& positions() { return positions_; }
    /// Iterator to the first s-position/state vector couple of the particle's trajectory
    PositionsMap::iterator begin() { return positions_.begin(); }
    /// Iterator to the last s-position/state vector couple of the particle's trajectory
    PositionsMap::iterator end() { return positions_.end(); }
    /// Iterator to the first s-position/state vector couple of the particle's trajectory
    const PositionsMap::const_iterator begin() const { return positions_.begin(); }
    /// Iterator to the last s-position/state vector couple of the particle's trajectory
    const PositionsMap::const_iterator end() const { return positions_.end(); }
    /// Iterator to the last s-position/state vector couple to the particle's trajectory
    const PositionsMap::const_reverse_iterator rbegin() const { return positions_.rbegin(); }

    /// Compute the value of the state vector at a given s-position
    StateVector stateVectorAt(double s) const;
    /// Get the particle's momentum at a given s-position
    const LorentzVector momentumAt(double s) const;

  private:
    int charge_;
    int pdgId_;
    bool physical_;
    bool stopped_;

    PositionsMap positions_;
  };

  /// Collection of particles composing the beam
  typedef std::vector<Particle> Particles;
}  // namespace hector

#endif
