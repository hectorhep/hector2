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

#ifndef Hector_Elements_Collimator_h
#define Hector_Elements_Collimator_h

#include "Hector/Elements/Drift.h"

namespace hector {
  namespace element {
    /// Collimator object builder
    /// \note Treated as a simple drift with a finite aperture opening
    class Collimator : public Drift {
    public:
      /// Class constructor
      Collimator(const std::string& name, double spos = 0., double length = 0.);

      std::shared_ptr<ElementBase> clone() const override { return std::make_shared<Collimator>(*this); }
    };
  }  // namespace element
}  // namespace hector

#endif
