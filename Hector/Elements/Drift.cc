#include "Drift.h"

namespace Hector {
  namespace Element {
    Drift::Drift(const std::string& name, double spos, double length) : ElementBase(aDrift, name, spos, length) {}

    Drift::Drift(const std::string& name, const Type& type, double spos, double length)
        : ElementBase(type, name, spos, length) {}

    Matrix Drift::matrix(double, double, int) const { return genericMatrix(length_); }

    Matrix Drift::genericMatrix(double length) {
      Matrix mat = DiagonalMatrix(6, 1);
      mat(1, 2) = length;
      mat(3, 4) = length;
      return mat;
    }
  }  // namespace Element
}  // namespace Hector
