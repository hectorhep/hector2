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

#ifndef Hector_Elements_Drift_h
#define Hector_Elements_Drift_h

#include "Hector/Elements/Element.h"
#include "Hector/Elements/ElementFwd.h"

namespace hector {
  namespace element {
    /// Drift object builder
    class Drift : public Element {
    public:
      explicit Drift(const std::string&, double spos = 0., double length = 0.);
      explicit Drift(const std::string&, const Type& type, double spos = 0., double length = 0.);

      ElementPtr clone() const override { return ElementPtr(new Drift(*this)); }
      Matrix matrix(double eloss = -1., double mp = -1., int qp = 0) const override;
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
  }  // namespace element
}  // namespace hector

#endif
