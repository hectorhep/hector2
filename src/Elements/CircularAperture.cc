#include "Hector/Elements/CircularAperture.h"

namespace hector {
  namespace aperture {
    CircularAperture::CircularAperture(double radius, const TwoVector& pos) : EllipticAperture(radius, radius, pos) {}

    CircularAperture::~CircularAperture() {}
  }  // namespace aperture
}  // namespace hector
