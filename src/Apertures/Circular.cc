#include "Hector/Apertures/Circular.h"

namespace hector {
  namespace aperture {
    Circular::Circular(double radius, const TwoVector& pos) : Elliptic(radius, radius, pos) {}

    Circular::~Circular() {}
  }  // namespace aperture
}  // namespace hector
