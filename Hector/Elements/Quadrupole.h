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

#ifndef Hector_Elements_Quadrupole_h
#define Hector_Elements_Quadrupole_h

#include "Hector/Elements/Element.h"

namespace hector {
  namespace element {
    /// A generic quadrupole object
    class Quadrupole : public Element {
    public:
      explicit Quadrupole(const Type& type, const std::string& name, double spos, double length, double mag_str)
          : Element(type, name, spos, length) {
        setMagneticStrength(mag_str);
      }
    };

    /// Horizontal quadrupole object builder
    class HorizontalQuadrupole : public Quadrupole {
    public:
      explicit HorizontalQuadrupole(const std::string& name, double spos, double length, double mag_str)
          : Quadrupole(anHorizontalQuadrupole, name, spos, length, mag_str) {}

      ElementPtr clone() const override { return ElementPtr(new HorizontalQuadrupole(*this)); }
      /** \note \f$
         * \mathbf{M} = \left(
         * \begin{array}{cccccc}
         * \cos{\omega} & \frac{1}{\sqrt{k}}\sin{\omega} & 0 & 0 & 0 & 0\\
         * -\sqrt{k}\sin{\omega} & \cos{\omega} & 0 & 0 & 0 & 0\\
         * 0 & 0 & \cosh{\omega} & \frac{1}{\sqrt{k}}\sinh{\omega} & 0 & 0\\
         * 0 & 0 & \sqrt{k}\sinh{\omega} & \cosh{\omega} & 0 & 0\\
         * 0 & 0 & 0 & 0 & 1 & 0\\
         * 0 & 0 & 0 & 0 & 0 & 1\\
         * \end{array}
         * \right)
         * \f$
         * assuming \f$ k =  k_{0} \cdot \frac{p_{0}}{p_{0} - \mathrm{d}p} \cdot \frac{q_{\mathrm{part}}}{q_{\mathrm{b}}} \f$ and \f$ \omega \equiv \omega(k,L) = L \sqrt{|k|} \f$
         */
      Matrix matrix(double,
                    double mp = Parameters::get().beamParticlesMass(),
                    int qp = Parameters::get().beamParticlesCharge()) const override;
    };

    /// Vertical quadrupole object builder
    class VerticalQuadrupole : public Quadrupole {
    public:
      explicit VerticalQuadrupole(const std::string& name, double spos, double length, double mag_str)
          : Quadrupole(aVerticalQuadrupole, name, spos, length, mag_str) {}

      ElementPtr clone() const override { return ElementPtr(new VerticalQuadrupole(*this)); }
      /** \note \f$
         * \mathbf{M} = \left(
         * \begin{array}{cccccc}
         * \cosh{\omega} & \frac{1}{\sqrt{k}}\sinh{\omega} & 0 & 0 & 0 & 0\\
         * \sqrt{k}\sinh{\omega} & \cosh{\omega} & 0 & 0 & 0 & 0\\
         * 0 & 0 & \cos{\omega} & \frac{1}{\sqrt{k}}\sin{\omega} & 0 & 0\\
         * 0 & 0 & -\sqrt{k}\sin{\omega} & \cos{\omega} & 0 & 0\\
         * 0 & 0 & 0 & 0 & 1 & 0\\
         * 0 & 0 & 0 & 0 & 0 & 1\\
         * \end{array}
         * \right)
         * \f$
         * assuming \f$ k =  k_{0} \cdot \frac{p_{0}}{p_{0} - \mathrm{d}p} \cdot \frac{q_{\mathrm{part}}}{q_{\mathrm{b}}} \f$ and \f$ \omega \equiv \omega(k,l) = L \sqrt{|k|} \f$
         */
      Matrix matrix(double,
                    double mp = Parameters::get().beamParticlesMass(),
                    int qp = Parameters::get().beamParticlesCharge()) const override;
    };
  }  // namespace element
}  // namespace hector

#endif
