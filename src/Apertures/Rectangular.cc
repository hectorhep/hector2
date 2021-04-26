#include "Hector/Apertures/Rectangular.h"

namespace hector {
  namespace aperture {
    Rectangular::Rectangular(double rect_length, double rect_height, const TwoVector& pos)
        : ApertureBase(aRectangularAperture, pos, {{rect_length, rect_height, 0., 0.}}) {}

    Rectangular::~Rectangular() {}

    bool Rectangular::contains(const TwoVector& pos) const {
      const TwoVector vec(pos - pos_);
      return (fabs(vec.x()) < p(0) && fabs(vec.y()) < p(1));
    }

    TwoVector Rectangular::limits() const { return TwoVector(p(0), p(1)); }
  }  // namespace aperture
}  // namespace hector
