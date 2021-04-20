#ifndef Hector_Elements_Kicker_h
#define Hector_Elements_Kicker_h

#include "ElementBase.h"

namespace hector {
  namespace element {
    /// A generic kicker object
    class Kicker : public ElementBase {
    public:
      /// (virtual) class constructor
      Kicker(const Type& type, const std::string& name, double spos, double length, double mag_str)
          : ElementBase(type, name, spos, length) {
        setMagneticStrength(mag_str);
      }
    };

    /// Horizontal kicker object builder
    class HorizontalKicker : public Kicker {
    public:
      /// Class constructor
      HorizontalKicker(const std::string& name, double spos, double length, double mag_str)
          : Kicker(anHorizontalKicker, name, spos, length, mag_str) {}

      std::shared_ptr<ElementBase> clone() const override { return std::make_shared<HorizontalKicker>(*this); }
      /** \note \f$
         * \mathbf{M} = \left(
         * \begin{array}{cccccc}
         * 1 & L & 0 & 0 & 0 & \frac{L}{2} \tan{k}\\
         * 0 & 1 & 0 & 0 & 0 & k\\
         * 0 & 0 & 1 & L & 0 & 0\\
         * 0 & 0 & 0 & 1 & 0 & 0\\
         * 0 & 0 & 0 & 0 & 1 & 0\\
         * 0 & 0 & 0 & 0 & 0 & 1\\
         * \end{array}
         * \right)
         * \f$
         * assuming \f$ k =  k_{0} \cdot \frac{p_{0}}{p_{0} - \mathrm{d}p} \cdot \frac{q_{\mathrm{particle}}}{q_{\mathrm{beam}}} \f$
         */
      Matrix matrix(double,
                    double mp = Parameters::get()->beamParticlesMass(),
                    int qp = Parameters::get()->beamParticlesCharge()) const override;
    };

    /// Vertical kicker object builder
    class VerticalKicker : public Kicker {
    public:
      /// Class constructor
      VerticalKicker(const std::string& name, double spos, double length, double mag_str)
          : Kicker(aVerticalKicker, name, spos, length, mag_str) {}

      std::shared_ptr<ElementBase> clone() const override { return std::make_shared<VerticalKicker>(*this); }
      /** \note \f$
         * \mathbf{M} = \left(
         * \begin{array}{cccccc}
         * 1 & L & 0 & 0 & 0 & 0\\
         * 0 & 1 & 0 & 0 & 0 & 0\\
         * 0 & 0 & 1 & L & 0 & \frac{L}{2} \tan{k}\\
         * 0 & 0 & 0 & 1 & 0 & k\\
         * 0 & 0 & 0 & 0 & 1 & 0\\
         * 0 & 0 & 0 & 0 & 0 & 1\\
         * \end{array}
         * \right)
         * \f$
         * assuming \f$ k =  k_{0} \cdot \frac{p_{0}}{p_{0} - \mathrm{d}p} \cdot \frac{q_{\mathrm{particle}}}{q_{\mathrm{beam}}} \f$
         */
      Matrix matrix(double,
                    double mp = Parameters::get()->beamParticlesMass(),
                    int qp = Parameters::get()->beamParticlesCharge()) const override;
    };
  }  // namespace element
}  // namespace hector

#endif
