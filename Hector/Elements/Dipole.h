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

#ifndef Hector_Elements_Dipole_h
#define Hector_Elements_Dipole_h

#include "Hector/Elements/Element.h"
#include "Hector/Elements/ElementFwd.h"

namespace hector {
  namespace element {
    /// A generic dipole object
    class Dipole : public Element {
    public:
      explicit Dipole(const Type& type, const std::string& name, double spos, double length, double mag_str)
          : Element(type, name, spos, length) {
        setMagneticStrength(mag_str);
      }
    };

    /// Rectangular dipole object builder
    class RectangularDipole : public Dipole {
    public:
      explicit RectangularDipole(const std::string& name, double spos, double length, double mag_str)
          : Dipole(aRectangularDipole, name, spos, length, mag_str) {}

      ElementPtr clone() const override { return std::make_shared<RectangularDipole>(*this); }
      /** \note \f$
         * \mathbf{M} = \left(
         * \begin{array}{cccccc}
         * \cos{\theta} & r \sin{\theta} & 0 & 0 & \frac{2r}{E_{\mathrm{b}}} \sin^2{\frac{\theta}{2}} & 0\\
         * \frac{-1}{r} \sin{\theta} & \cos{\theta} & 0 & 0 & \frac{1}{E_{\mathrm{b}}}\sin{\theta} & 0\\
         * 0 & 0 & 1 & L & 0 & 0\\
         * 0 & 0 & 0 & 1 & 0 & 0\\
         * 0 & 0 & 0 & 0 & 1 &0\\
         * 0 & 0 & 0 & 0 & 0 & 1\\
         * \end{array}
         * \right)
         * \f$
         * assuming \f$\theta = {L\over r}\f$, \f$ {1\over r} \equiv k = k_{0} \cdot \frac{p_{0}}{p_{0} - \mathrm{d}p} \cdot \frac{q_{\mathrm{part}}}{q_{\mathrm{b}}} \f$, and \f$ E_{\mathrm{b}}\f$ = 7000 GeV.
         */
      /// \note Numerical sensitivity (~\f$10^{-8}\f$ relative precision on a 64-bit Intel machine) expected with \f$ \frac{r}{E_{\mathrm{b}}} \left(1-\cos{\theta}\right)\f$.
      ///  Using \f$ \cos{2x} = 1-2\sin^{2}{x} \f$ to transform this term (see the variable called "simp")
      Matrix matrix(double, double mp = -1., int qp = 0) const override;
    };

    /// Sector dipole object builder
    class SectorDipole : public Dipole {
    public:
      explicit SectorDipole(const std::string& name, double spos, double length, double mag_str)
          : Dipole(aSectorDipole, name, spos, length, mag_str) {}

      ElementPtr clone() const override { return std::make_shared<SectorDipole>(*this); }

      /// \note Matrix depends if the bending is on or off.
      /** \note \f$
         * \mathbf{M}_{\mathrm{no~bend.}} = \left(
         * \begin{array}{cccccc}
         * \cos{\theta} & r \sin{\theta} & 0 & 0 & 0 & 0\\
         * \frac{-1}{r} \sin{\theta} & \cos{\theta} & 0 & 0 & 0 & 0\\
         * 0 & 0 & 1 & L & 0 & 0\\
         * 0 & 0 & 0 & 1 & 0 & 0\\
         * 0 & 0 & 0 & 0 & 1 & 0\\
         * 0 & 0 & 0 & 0 & 0 & 1\\
         * \end{array}
         * \right),
         * \f$\f$
         * \mathbf{M}_{\mathrm{bend.}} = \left(
         * \begin{array}{cccccc}
         * \cos{\theta} & r \sin{\theta} & 0 & 0 & \frac{2r}{E_{\mathrm{b}}} \sin^2{\frac{\theta}{2}} & 0\\
         * \frac{-1}{r} \sin{\theta} & \cos{\theta} & 0 & 0 & \frac{1}{E_{\mathrm{b}}} \sin{\theta} & 0\\
         * 0 & 0 & 1 & L & 0 &0\\
         * 0 & 0 & 0 & 1 & 0 &0\\
         * 0 & 0 & 0 & 0 & 1 & 0\\
         * 0 & 0 & 0 & 0 & 0 & 1\\
         * \end{array}
         * \right)
         * \f$
         * assuming \f$\theta = {L\over r}\f$, \f$ {1\over r} \equiv k =  k_{0} \cdot \frac{p_{0}}{p_{0} - \mathrm{d}p} \cdot \frac{q_{\mathrm{part}}}{q_{\mathrm{b}}} \f$
         */
      Matrix matrix(double, double mp = -1., int qp = 0) const override;
    };
  }  // namespace element
}  // namespace hector

#endif
