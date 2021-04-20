#include "RectEllipticAperture.h"

namespace Hector {
  namespace Aperture {
    RectEllipticAperture::RectEllipticAperture(
        double rect_size_x, double rect_size_y, double ell_size_x, double ell_size_y, const TwoVector& pos)
        : ApertureBase(aRectEllipticAperture, pos, {{rect_size_x, rect_size_y, ell_size_x, ell_size_y}}) {}

    RectEllipticAperture::~RectEllipticAperture() {}

    bool RectEllipticAperture::contains(const TwoVector& pos) const {
      const TwoVector vec1(pos - pos_), vec2(vec1.x() / p(2), vec1.y() / p(3));
      return ((fabs(vec1.x()) < p(0)) && (fabs(vec1.y()) < p(1))  // rectangular part
              && (vec2.mag2() < 1.));                             // elliptic part
    }

    TwoVector RectEllipticAperture::limits() const {  //FIXME
      return TwoVector(std::min(p(0), p(2)), std::min(p(1), p(3)));
    }
  }  // namespace Aperture
}  // namespace Hector
