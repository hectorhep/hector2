#ifndef Hector_Elements_Drift_h
#define Hector_Elements_Drift_h

#include "ElementBase.h"

namespace Hector {
  namespace Element {
    /// Drift object builder
    class Drift : public ElementBase {
    public:
      /// Class constructor
      Drift(const std::string&, double spos = 0., double length = 0.);
      /// Class constructor
      Drift(const std::string&, const Type& type, double spos = 0., double length = 0.);

      std::shared_ptr<ElementBase> clone() const override { return std::make_shared<Drift>(*this); }
      Matrix matrix(double eloss = -1.,
                    double mp = Parameters::get()->beamParticlesMass(),
                    int qp = Parameters::get()->beamParticlesCharge()) const override;
      /// Build a transfer matrix for a given drift length
      /// \param[in] length drift length
      /** \note \f$
         * \mathbf{M} = \left(
         * \begin{array}{cccccc}
         * 1 & L & 0 & 0 & 0 & 0\\
         * 0 & 1 & 0 & 0 & 0 & 0\\
         * 0 & 0 & 1 & L & 0 & 0\\
         * 0 & 0 & 0 & 1 & 0 & 0\\
         * 0 & 0 & 0 & 0 & 1 & 0\\
         * 0 & 0 & 0 & 0 & 0 & 1\\
         * \end{array}
         * \right)
         * \f$
         */
      static Matrix genericMatrix(double length);
    };
  }  // namespace Element
}  // namespace Hector

#endif
