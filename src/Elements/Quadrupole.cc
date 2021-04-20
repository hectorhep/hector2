#include "Hector/Elements/Quadrupole.h"
#include "Hector/Elements/Drift.h"
#include "Hector/Exception.h"

namespace hector {
  namespace element {
    Matrix HorizontalQuadrupole::matrix(double eloss, double mp, int qp) const {
      Matrix mat = Drift::genericMatrix(length_);

      const double ke = fieldStrength(eloss, mp, qp);  // should be negative
      if (ke > 0.)
        throw Exception(__PRETTY_FUNCTION__, Fatal)
            << "Magnetic strength for horizontal quadrupole " << name_ << " should be negative!\n\t"
            << "Value = " << ke << ".";
      if (ke == 0.) {  // simple drift matrix
        PrintDebug << "Quadrupole " << name_ << " has no effect. Treating it as a drift.";
        return mat;
      }

      const double sq_k = sqrt(-ke), inv_sq_k = 1. / sq_k;
      const double omega = sq_k * length_;
      const double s_omega = sin(omega), c_omega = cos(omega), sh_omega = sinh(omega), ch_omega = cosh(omega);

      // Focussing Twiss matrix for the horizontal component
      mat(1, 1) = c_omega;
      mat(1, 2) = s_omega * inv_sq_k;
      mat(2, 1) = s_omega * (-sq_k);
      mat(2, 2) = c_omega;
      // Defocussing Twiss matrix for the vertical component
      mat(3, 3) = ch_omega;
      mat(3, 4) = sh_omega * inv_sq_k;
      mat(4, 3) = sh_omega * sq_k;
      mat(4, 4) = ch_omega;
      return mat;
    }

    Matrix VerticalQuadrupole::matrix(double eloss, double mp, int qp) const {
      Matrix mat = Drift::genericMatrix(length_);

      const double ke = fieldStrength(eloss, mp, qp);
      if (ke < 0.)
        throw Exception(__PRETTY_FUNCTION__, Fatal)
            << "Magnetic strength for vertical quadrupole " << name_ << " should be positive!\n\t"
            << "Value = " << ke << ".";
      if (ke == 0.) {  // simple drift matrix
        PrintDebug << "Quadrupole " << name_ << " has no effect. Treating it as a drift.";
        return mat;
      }

      const double sq_k = sqrt(ke), inv_sq_k = 1. / sq_k;
      const double omega = sq_k * length_;
      const double s_omega = sin(omega), c_omega = cos(omega), sh_omega = sinh(omega), ch_omega = cosh(omega);

      // Defocussing Twiss matrix for the horizontal component
      mat(1, 1) = ch_omega;
      mat(1, 2) = sh_omega * inv_sq_k;
      mat(2, 1) = sh_omega * sq_k;
      mat(2, 2) = ch_omega;
      // Focussing Twiss matrix for the vertical component
      mat(3, 3) = c_omega;
      mat(3, 4) = s_omega * inv_sq_k;
      mat(4, 3) = s_omega * (-sq_k);
      mat(4, 4) = c_omega;
      return mat;
    }
  }  // namespace element
}  // namespace hector
