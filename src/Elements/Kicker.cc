#include "Hector/Elements/Kicker.h"
#include "Hector/Elements/Drift.h"
#include "Hector/Core/Parameters.h"

namespace hector {
  namespace element {
    Matrix HorizontalKicker::matrix(double eloss, double mp, int qp) const {
      Matrix mat = Drift::genericMatrix(length_);

      if (!Parameters::get()->enableKickers())
        return mat;

      const double ke = -fieldStrength(eloss, mp, qp);
      if (ke == 0.)
        return mat;

      mat(1, 6) = length_ * tan(ke) * 0.5;
      mat(2, 6) = ke;
      return mat;
    }

    Matrix VerticalKicker::matrix(double eloss, double mp, int qp) const {
      Matrix mat = Drift::genericMatrix(length_);

      if (!Parameters::get()->enableKickers())
        return mat;

      const double ke = -fieldStrength(eloss, mp, qp);
      if (ke == 0.)
        return mat;

      mat(3, 6) = length_ * tan(ke) * 0.5;
      mat(4, 6) = ke;
      return mat;
    }
  }  // namespace element
}  // namespace hector
