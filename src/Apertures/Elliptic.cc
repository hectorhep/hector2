#include "Hector/Apertures/Elliptic.h"

namespace hector {
  namespace aperture {
    Elliptic::Elliptic(double ell_length, double ell_height, const TwoVector& pos)
        : ApertureBase(anEllipticAperture, pos, {{ell_length, ell_height, 0.f, 0.f}}) {}

    Elliptic::~Elliptic() {}

    bool Elliptic::contains(const TwoVector& pos) const {
      const TwoVector vec((pos.x() - pos_.x()) / p(0), (pos.y() - pos_.y()) / p(1));
      return (vec.norm() < 1.);
    }

    TwoVector Elliptic::limits() const { return TwoVector(p(0), p(1)); }
  }  // namespace aperture
}  // namespace hector
