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

#ifndef Hector_Apertures_Rectangular_h
#define Hector_Apertures_Rectangular_h

#include "Hector/Apertures/ApertureBase.h"

namespace hector {
  namespace aperture {
    /// Rectangular shape aperture
    class Rectangular : public ApertureBase {
    public:
      /// Class constructor
      Rectangular(double, double, const TwoVector& pos = TwoVector());
      ~Rectangular();

      std::shared_ptr<ApertureBase> clone() const override { return std::make_shared<Rectangular>(*this); }

      bool contains(const TwoVector&) const override;
      TwoVector limits() const override;
    };
  }  // namespace aperture
}  // namespace hector

#endif
