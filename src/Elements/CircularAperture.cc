#include "Hector/Elements/CircularAperture.h"

namespace Hector {
  namespace Aperture {
    CircularAperture::CircularAperture(double radius, const TwoVector& pos) : EllipticAperture(radius, radius, pos) {}

    CircularAperture::~CircularAperture() {}
  }  // namespace Aperture
}  // namespace Hector
