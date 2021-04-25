#ifndef Hector_Core_Algebra_h
#define Hector_Core_Algebra_h

#include <Eigen/Dense>

#include <array>
#include <vector>

namespace Hector {
  /// A generic N-dimensional matrix
  typedef Eigen::MatrixXf Matrix;
  /// A generic N-dimensional diagonal matrix
  typedef Eigen::MatrixXf DiagonalMatrix;

  /// N-dimensional vector of double-precision floats
  class Vector : public Eigen::VectorXf {
  public:
    using Eigen::VectorXf::VectorXf;
    Vector() : Eigen::VectorXf() {}
    /// Build a N-vector from an equivalent CLHEP object
    Vector(const Eigen::VectorXf vec) : Eigen::VectorXf(vec) {}
    /// Build a N-vector from its N-dimensional coordinates
    Vector(const std::vector<double>& vec) : Eigen::VectorXf(vec.size()) {
      unsigned short i = 0;
      for (const auto& c : vec) {
        operator()(i) = c;
        ++i;
      }
    }
  };
  /// Two-vector of double-precision floats
  class TwoVector : public Eigen::Vector2f {
  public:
    using Eigen::Vector2f::Vector2f;
    TwoVector() : Eigen::Vector2f() {}
    /// Build a two-vector from an equivalent CLHEP object
    TwoVector(const Eigen::Vector2f& vec) : Eigen::Vector2f(vec) {}
    /// Build a two-vector from its two-dimensional spatial coordinates
    TwoVector(const std::array<double, 2>& vec) : Eigen::Vector2f(vec[0], vec[1]) {}
  };
  /// Three-vector of double-precision floats
  class ThreeVector : public Eigen::Vector3f {
  public:
    using Eigen::Vector3f::Vector3f;
    ThreeVector() : Eigen::Vector3f() {}
    /// Build a three-vector from an equivalent CLHEP object
    ThreeVector(const Eigen::Vector3f& vec) : Eigen::Vector3f(vec) {}
    /// Build a three-vector from its spatial coordinates
    ThreeVector(const std::array<double, 3>& vec) : Eigen::Vector3f(vec[0], vec[1], vec[2]) {}
  };
  /// Lorentz vector of double-precision floats
  class LorentzVector : public Eigen::Vector4f {
  public:
    using Eigen::Vector4f::Vector4f;
    LorentzVector() : Eigen::Vector4f() {}
    /// Build a Lorentz vector from an equivalent CLHEP object
    LorentzVector(const Eigen::Vector4f& vec) : Eigen::Vector4f(vec) {}
    /// Build a Lorentz vector from its spatial and temporal coordinates
    LorentzVector(const std::array<double, 3>& sp, double t) : Eigen::Vector4f(sp[0], sp[1], sp[2], t) {}
    /// Build a Lorentz vector from a four-vector containing its spatial and temporal coordinates
    LorentzVector(const std::array<double, 4> vec) : Eigen::Vector4f(vec[0], vec[1], vec[2], vec[3]) {}
    double m() const;
  };

  namespace math {
    /// Compute the tangent of both the components of a 2-vector
    TwoVector tan2(const TwoVector& ang);
    /// Compute the arc-tangent of both the components of a 2-vector
    TwoVector atan2(const TwoVector& ang);
  }  // namespace math
}  // namespace Hector

#endif
