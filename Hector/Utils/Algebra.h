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

#ifndef Hector_Utils_Algebra_h
#define Hector_Utils_Algebra_h

#include <Eigen/Dense>
#include <array>

namespace hector {
  /// A generic N-dimensional matrix
  typedef Eigen::MatrixXf Matrix;
  /// A generic N-dimensional diagonal matrix
  typedef Eigen::MatrixXf DiagonalMatrix;

  /// N-dimensional vector of double-precision floats
  class Vector : public Eigen::VectorXf {
  public:
    using Eigen::VectorXf::VectorXf;
    /// Build a N-vector from its N-dimensional coordinates
    explicit Vector(const std::initializer_list<double>& vec) : Eigen::VectorXf(vec.size()) {
      unsigned short i = 0;
      for (const auto& c : vec)
        operator()(i++) = c;
    }
  };
  /// Two-vector of double-precision floats
  class TwoVector : public Eigen::Vector2f {
  public:
    using Eigen::Vector2f::Vector2f;
    /// Build a two-vector from its two-dimensional spatial coordinates
    explicit TwoVector(const std::initializer_list<double>& vec) : Eigen::Vector2f(*vec.begin(), *(vec.begin() + 1)) {}
    /// Set the horizontal component
    void setX(double x) { (*this)[0] = x; }
    /// Set the vertical component
    void setY(double y) { (*this)[1] = y; }
  };
  /// Three-vector of double-precision floats
  class ThreeVector : public Eigen::Vector3f {
  public:
    using Eigen::Vector3f::Vector3f;
    /// Build a three-vector from its spatial coordinates
    explicit ThreeVector(const std::initializer_list<double>& vec)
        : Eigen::Vector3f(*vec.begin(), *(vec.begin() + 1), *(vec.begin() + 2)) {}
    /// Set the horizontal component
    void setX(double x) { (*this)[0] = x; }
    /// Set the vertical component
    void setY(double y) { (*this)[1] = y; }
    /// Set the longitudinal component
    void setZ(double z) { (*this)[2] = z; }
  };
  /// Lorentz vector of double-precision floats
  class LorentzVector : public Eigen::Vector4f {
  public:
    using Eigen::Vector4f::Vector4f;
    /// Build a Lorentz vector from its spatial and temporal coordinates
    explicit LorentzVector(const std::array<double, 3>& sp, double t) : Eigen::Vector4f(sp[0], sp[1], sp[2], t) {}
    /// Build a Lorentz vector from a four-vector containing its spatial and temporal coordinates
    explicit LorentzVector(const std::array<double, 4> vec) : Eigen::Vector4f(vec[0], vec[1], vec[2], vec[3]) {}
    /// Set the horizontal momentum component
    void setX(double x) { (*this)[0] = x; }
    /// Set the vertical momentum component
    void setY(double y) { (*this)[1] = y; }
    /// Set the longitudinal momentum component
    void setZ(double z) { (*this)[2] = z; }
    /// Set the time/energy component
    void setT(double t) { (*this)[3] = t; }
    /// Four-momenta multiplication operator
    double operator*(const LorentzVector& oth);
    /// Four-momentum norm/Lorentz vector mass
    double m() const;
  };

  namespace math {
    /// Compute the tangent of both the components of a 2-vector
    TwoVector tan2(const TwoVector& ang);
    /// Compute the arc-tangent of both the components of a 2-vector
    TwoVector atan2(const TwoVector& ang);
  }  // namespace math
}  // namespace hector

#endif
