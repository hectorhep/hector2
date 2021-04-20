#include "Hector/Elements/Collimator.h"

namespace hector {
  namespace element {
    Collimator::Collimator(const std::string& name, double spos, double length) : Drift(name, spos, length) {}
  }  // namespace element
}  // namespace hector
