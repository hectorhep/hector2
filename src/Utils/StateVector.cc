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
#include "Hector/Utils/Kinematics.h"
#include "Hector/Utils/StateVector.h"
#include "Hector/Utils/String.h"

namespace hector {
  StateVector::StateVector() : Vector(6, 0), m_(0.) {
    (*this)[K] = 1.;
    (*this)[E] = Parameters::get().beamEnergy();
  }

  StateVector::StateVector(const Vector& vec, double mass) : Vector(vec), m_(mass) {}

  StateVector::StateVector(const LorentzVector& mom, const TwoVector& pos) : Vector(6, 1), m_(mom.m()) {
    setPosition(pos);
    setMomentum(mom);
    (*this)[K] = 1.;
  }

  StateVector::StateVector(const TwoVector& pos, const TwoVector& ang, double energy, double kick)
      : Vector(6, 1), m_(0.) {
    setPosition(pos);
    setAngles(ang);
    if (energy < 0.)
      energy = Parameters::get().beamEnergy();
    setEnergy(energy);
    setKick(kick);
  }

  void StateVector::setXi(double xi) { setEnergy(xi_to_e(xi)); }

  double StateVector::xi() const { return e_to_xi(energy()); }

  void StateVector::setPosition(double x, double y) {
    // store in m
    (*this)[X] = x;
    (*this)[Y] = y;
  }

  TwoVector StateVector::position() const {
    // return in m
    return TwoVector((*this)[X], (*this)[Y]);
  }

  void StateVector::setAngles(double tx, double ty) {
    (*this)[TX] = tx;
    (*this)[TY] = ty;
  }

  TwoVector StateVector::angles() const {
    // return in rad
    return TwoVector((*this)[TX], (*this)[TY]);
  }

  void StateVector::setMomentum(const LorentzVector& mom) {
    setAngles(mom.x() / mom.z(), mom.y() / mom.z());
    setEnergy(mom.w());
    m_ = mom.m();
  }

  void StateVector::addMomentum(const LorentzVector& mom) {
    setAngles(angles() + TwoVector(mom.x() / mom.z(), mom.y() / mom.z()));
    (*this)[E] = mom.w();
    m_ = mom.m();
  }

  LorentzVector StateVector::momentum() const {
    const TwoVector tan_ang(math::tan2(angles()));
    const double pz = std::sqrt((energy() * energy() - m_ * m_) / (1. + std::pow(tan_ang.norm(), 2))),
                 px = pz * tan_ang.x(), py = pz * tan_ang.y();
    return LorentzVector(px, py, pz, energy());
  }

  void StateVector::setM(double mass) {
    if (mass != momentum().m()) {
      m_ = mass;
      if (momentum().norm() != 0.)
        (*this)[E] = std::hypot(momentum().norm(), m_);  // match the energy accordingly
      else {
        const int sign = 1;  //FIXME
        setMomentum(LorentzVector(
            0., 0., sign * std::sqrt(energy() * energy() - m_ * m_), energy()));  // longitudinal momentum only
      }
      return;
    }
    m_ = mass;
  }

  /// Human-readable printout of a state vector
  std::ostream& operator<<(std::ostream& os, const StateVector& vec) {
    return os << format("{ x = % 6.4e m, x' = % 6.3e rad, y = % 6.4e m, y' = % 6.3e rad, k = % 3.1f, E = %6.3e GeV }",
                        vec.position().x(),
                        vec.angles().x(),
                        vec.position().y(),
                        vec.angles().y(),
                        vec.kick(),
                        vec.energy());
  }
}  // namespace hector
