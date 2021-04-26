#include "Hector/Elements/Drift.h"
#include "Hector/Utils/StateVector.h"

namespace hector {
  namespace element {
    Drift::Drift(const std::string& name, double spos, double length) : ElementBase(aDrift, name, spos, length) {}

    Drift::Drift(const std::string& name, const Type& type, double spos, double length)
        : ElementBase(type, name, spos, length) {}

    Matrix Drift::matrix(double, double, int) const { return genericMatrix(length_); }

    Matrix Drift::genericMatrix(double length) {
      Matrix mat = DiagonalMatrix(6, 1);
      mat(StateVector::X, StateVector::TX) = length;
      mat(StateVector::Y, StateVector::TY) = length;
      return mat;
    }
  }  // namespace element
}  // namespace hector
