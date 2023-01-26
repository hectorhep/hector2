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

#include <sstream>

#include "Hector/Apertures/Aperture.h"
#include "Hector/Utils/String.h"

namespace hector {
  namespace aperture {
    Aperture::Aperture(const Type& type, const TwoVector& pos, const Parameters& param)
        : type_(type), pos_(pos), param_(param) {}

    Aperture::~Aperture() { param_.clear(); }

    const std::string Aperture::typeName() const {
      std::ostringstream os;
      os << type_;
      return os.str();
    }

    bool Aperture::operator==(const Aperture& rhs) const {
      if (type_ != rhs.type_)
        return false;
      if (pos_ != rhs.pos_)
        return false;
      if (param_ != rhs.param_)
        return false;
      return true;
    }
  }  // namespace aperture

  std::ostream& operator<<(std::ostream& os, const aperture::Aperture& ap) {
    os << ap.type() << format(" (param. (%.3f, %.3f, %.3f, %.3f),", ap.p(0), ap.p(1), ap.p(2), ap.p(3)) << " center at "
       << ap.position() << ")";
    return os;
  }

  std::ostream& operator<<(std::ostream& os, const aperture::Aperture* ap) {
    os << *(ap);
    return os;
  }
}  // namespace hector
