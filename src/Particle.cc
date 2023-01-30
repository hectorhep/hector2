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

#include "Hector/Exception.h"
#include "Hector/Parameters.h"
#include "Hector/Particle.h"
#include "Hector/Utils/String.h"

namespace hector {
  Particle::Particle() : charge_(0), pdgId_(0), physical_(true), stopped_(false) { addPosition(0., StateVector()); }

  Particle::Particle(const StateVector& sv0, double s0) : charge_(0), pdgId_(0), physical_(true), stopped_(false) {
    addPosition(s0, sv0);
  }

  Particle::Particle(const LorentzVector& mom, int charge, int pdgid)
      : charge_(charge == 999 ? Parameters::get().beamParticlesCharge() : charge),
        pdgId_(pdgid),
        physical_(true),
        stopped_(false) {
    addPosition(0., StateVector(mom));
  }

  Particle::~Particle() {}

  Particle Particle::fromMassCharge(double mass, int charge) {
    Particle p(StateVector(Vector(6, 0), mass));
    p.setCharge(charge);
    return p;
  }

  void Particle::addPosition(const Position& pos, bool stopped) {
    if (!positions_.empty() > 0 && lastStateVector().m() != pos.stateVector().m())
      throw H_ERROR << "Particle mass is not conserved in propagation!\n\t"
                    << "Previous mass was " << lastStateVector().m() << " GeV, new mass is " << pos.stateVector().m()
                    << " GeV.";
    positions_.insert(pos.pair());
    stopped_ = stopped;
  }

  StateVector Particle::stateVectorAt(double s) const {
    const auto& pos_s = positions_.find(s);
    if (pos_s != positions_.end())
      return pos_s->second;

    const auto &lower_it = --positions_.upper_bound(s), upper_it = positions_.upper_bound(s);

    if (lower_it == positions_.end() || upper_it->first < lower_it->first)
      throw H_ERROR << "Impossible to interpolate the position at s = " << s << " m.";

    //PrintInfo( Form( "Interpolating for s = %.2f between %.2f and %.2f", s, lower_it->first, upper_it->first ) );

    const StateVector &sv_before = lower_it->second, &sv_after = upper_it->second;
    const TwoVector &in = sv_before.position(), &out = sv_after.position();

    const double drift_length = upper_it->first - lower_it->first;
    if (drift_length == 0)
      throw H_ERROR << "No luck in choosing position (s=" << s << " m)\n\t"
                    << "Interpolation is impossible!";

    const TwoVector s_pos = in + ((s - lower_it->first) / drift_length) * (out - in);

    StateVector out_stvec(lower_it->second);
    out_stvec.setPosition(s_pos);
    return out_stvec;
  }

  const LorentzVector Particle::momentumAt(double s) const { return stateVectorAt(s).momentum(); }

  void Particle::dump(std::ostream& os) const {
    os << "Particle of charge " << charge_ << "\n"
       << " initial position: " << firstStateVector() << "\n";
    if (positions_.size() == 1)
      return;
    os << " list of associated state vectors:\n";
    for (const auto& pos : positions_)
      os << format("   s = %8.3f m:", pos.first) << " " << pos.second << std::endl;
  }
}  // namespace hector
