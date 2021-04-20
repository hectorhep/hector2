#include "Collimator.h"

namespace Hector {
  namespace Element {
    Collimator::Collimator(const std::string& name, double spos, double length) : Drift(name, spos, length) {}
  }  // namespace Element
}  // namespace Hector
