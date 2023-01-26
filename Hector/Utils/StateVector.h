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

#ifndef Hector_Utils_StateVector_h
#define Hector_Utils_StateVector_h

#include "Hector/Utils/Algebra.h"

namespace hector {
  /// Six-dimensional state vector associated to a particle at a given s
  class StateVector : private Vector {
  public:
    /// Human-readable enumeration of the 6 state vector coordinates
    enum Components { X = 0, TX = 1, Y = 2, TY = 3, E = 4, K = 5 };

  public:
    /// Build a blank state
    StateVector();
    //StateVector( const StateVector& sv ) : TwoVector( sv.vector() ), m_( sv.m_ ) {}
    /// Build a state using a 6-component vector and a particle mass
    /// \param[in] vec A 6-component vector
    /// \param[in] mass Particle mass (GeV)
    StateVector(const Vector& vec, double mass);
    /// Build a state using a particle kinematics and position
    /// \param[in] mom Four-momentum of the particle (GeV)
    /// \param[in] pos x-y position of the particle (m)
    StateVector(const LorentzVector& mom, const TwoVector& pos = TwoVector());
    /// Build a state vector using the particle's position and its angle
    StateVector(const TwoVector& pos, const TwoVector& ang, double energy = -1., double kick = 1.);
    ~StateVector() {}

    /// Get the 6-vector associated to this state
    const Vector& vector() const { return *this; }

    /// Set the particle energy (in GeV)
    void setEnergy(double energy) { (*this)[E] = energy; }
    /// Particle energy (in GeV)
    double energy() const { return (*this)[E]; }
    /// Set the energy loss \f$ \xi \f$
    void setXi(double xi);
    /// Energy loss \f$ \xi \f$
    double xi() const;
    /// Set the particle kick
    void setKick(double kick) { (*this)[K] = kick; }
    /// Particle kick
    double kick() const { return (*this)[K]; }

    /// Fill the components of a state according to the particle position
    void setPosition(double x, double y);
    /// Fill the components of a state according to the particle position
    void setPosition(const TwoVector& pos) { setPosition(pos.x(), pos.y()); }
    /// x-y position of a particle (in m)
    TwoVector position() const;
    /// Set the horizontal position (in m)
    void setX(double x) { (*this)[X] = x; }
    /// Horizontal position (in m)
    double x() const { return (*this)[X]; }
    /// Set the vertical position (in m)
    void setY(double y) { (*this)[Y] = y; }
    /// Vertical position (in m)
    double y() const { return (*this)[Y]; }

    /// Fill the components of a state according to the particle x'-y' angles (in rad)
    void setAngles(double tx, double ty);
    /// Fill the components of a state according to the particle x'-y' angles (in rad)
    void setAngles(const TwoVector& angles) { setAngles(angles.x(), angles.y()); }
    /// x'-y' polar angles of a particles (in rad)
    TwoVector angles() const;
    /// Set the horizontal angle (in rad)
    void setTx(double tx) { (*this)[TX] = tx; }
    /// Horizontal angle (in rad)
    double Tx() const { return angles().x(); }
    /// Set the vertical angle (in rad)
    void setTy(double ty) { (*this)[TY] = ty; }
    /// Vertical angle (in rad)
    double Ty() const { return angles().y(); }

    /// Fill the components of a state according to the particle kinematics
    void setMomentum(const LorentzVector& mom);
    void addMomentum(const LorentzVector& mom);
    /// Four-momentum of the particle (in GeV)
    LorentzVector momentum() const;
    /// Set the particle mass (in GeV)
    /// \warning Use with care! edits the energy as well as the mass to maintain a consistant kinematics.
    void setM(double mass);
    /// Particle mass (in GeV)
    double m() const { return m_; }

  private:
    double m_;
  };
  /// Human-readable printout of the state vector
  std::ostream& operator<<(std::ostream&, const StateVector&);
}  // namespace hector

#endif
