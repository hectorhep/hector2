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

#ifndef Hector_Apertures_Elliptic_h
#define Hector_Apertures_Elliptic_h

#include "Hector/Apertures/Aperture.h"

namespace hector {
  namespace aperture {
    /// Elliptic shape aperture
    class Elliptic : public Aperture {
    public:
      /// Class constructor
      explicit Elliptic(double, double, const TwoVector& = TwoVector(0., 0.));
      AperturePtr clone() const override { return std::make_shared<Elliptic>(*this); }

      bool contains(const TwoVector&) const override;
      TwoVector limits() const override;
    };
  }  // namespace aperture
}  // namespace hector

#endif
