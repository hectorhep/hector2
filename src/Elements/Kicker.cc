#include "Hector/Elements/Kicker.h"
#include "Hector/Elements/Drift.h"
#include "Hector/Parameters.h"
#include "Hector/Utils/StateVector.h"

namespace hector {
  namespace element {
    Matrix HorizontalKicker::matrix(double eloss, double mp, int qp) const {
      Matrix mat = Drift::genericMatrix(length_);

      if (!Parameters::get()->enableKickers())
        return mat;

      const double ke = -fieldStrength(eloss, mp, qp);
      if (ke == 0.)
        return mat;

      mat(StateVector::X, StateVector::K) = length_ * tan(ke) * 0.5;
      mat(StateVector::TX, StateVector::K) = ke;
      return mat;
    }

    Matrix VerticalKicker::matrix(double eloss, double mp, int qp) const {
      Matrix mat = Drift::genericMatrix(length_);

      if (!Parameters::get()->enableKickers())
        return mat;

      const double ke = -fieldStrength(eloss, mp, qp);
      if (ke == 0.)
        return mat;

      mat(StateVector::Y, StateVector::K) = length_ * tan(ke) * 0.5;
      mat(StateVector::TY, StateVector::K) = ke;
      return mat;
    }
  }  // namespace element
}  // namespace hector
