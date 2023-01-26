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

#ifndef Hector_Apertures_RectElliptic_h
#define Hector_Apertures_RectElliptic_h

#include "Hector/Apertures/Aperture.h"

namespace hector {
  namespace aperture {
    /// Rectangular/elliptic shape aperture
    class RectElliptic : public Aperture {
    public:
      explicit RectElliptic(double, double, double, double, const TwoVector& = TwoVector());

      AperturePtr clone() const override { return AperturePtr(new RectElliptic(*this)); }

      bool contains(const TwoVector&) const override;
      TwoVector limits() const override;
    };
  }  // namespace aperture
}  // namespace hector

#endif
