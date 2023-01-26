/*
 *  Hector: a beamline propagation tool
 *  Copyright (C) 2016-2023  Laurent Forthomme
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef Hector_Elements_Kicker_h
#define Hector_Elements_Kicker_h

#include "Hector/Elements/Element.h"

namespace hector {
  namespace element {
    /// A generic kicker object
    class Kicker : public Element {
    public:
      explicit Kicker(const Type& type, const std::string& name, double spos, double length, double mag_str)
          : Element(type, name, spos, length) {
        setMagneticStrength(mag_str);
      }
    };

    /// Horizontal kicker object builder
    class HorizontalKicker : public Kicker {
    public:
      explicit HorizontalKicker(const std::string& name, double spos, double length, double mag_str)
          : Kicker(anHorizontalKicker, name, spos, length, mag_str) {}

      ElementPtr clone() const override { return ElementPtr(new HorizontalKicker(*this)); }
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
      explicit VerticalKicker(const std::string& name, double spos, double length, double mag_str)
          : Kicker(aVerticalKicker, name, spos, length, mag_str) {}

      ElementPtr clone() const override { return ElementPtr(new VerticalKicker(*this)); }
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
