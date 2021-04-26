#include "Hector/Utils/Algebra.h"

namespace hector {
  namespace math {
    /// Compute the tangent of both the components of a 2-vector
    TwoVector tan2(const TwoVector& ang) {
      const double x = ang.x(), y = ang.y();
      return TwoVector(tan(x), tan(y));
    }

    /// Compute the arc-tangent of both the components of a 2-vector
    TwoVector atan2(const TwoVector& ang) {
      const double x = ang.x(), y = ang.y();
      return TwoVector(atan(x), atan(y));
    }
  }  // namespace math

  double LorentzVector::operator*(const LorentzVector& oth) {
    return w() * oth.w() - x() * oth.x() - y() * oth.y() - z() * oth.z();
  }

  double LorentzVector::m() const {
    const double m2 = LorentzVector(*this) * (*this);
    if (m2 < 0.)
      return -std::sqrt(-m2);
    return std::sqrt(m2);
  }
}  // namespace hector
