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

#include "Hector/Elements/Element.h"
#include "Hector/Exception.h"
#include "Hector/Parameters.h"
#include "Hector/Utils/String.h"

namespace hector {
  namespace element {
    Element::Element(const Type& type, const std::string& name, double spos, double length)
        : type_(type), name_(name), length_(length), magnetic_strength_(0.), s_(spos) {}

    Element::Element(Element& rhs)
        : type_(rhs.type_),
          name_(rhs.name_),
          aperture_(rhs.aperture_),
          length_(rhs.length_),
          magnetic_strength_(rhs.magnetic_strength_),
          pos_(rhs.pos_),
          angles_(rhs.angles_),
          s_(rhs.s_),
          beta_(rhs.beta_),
          disp_(rhs.disp_),
          rel_pos_(rhs.rel_pos_) {}

    Element::Element(const Element& rhs)
        : type_(rhs.type_),
          name_(rhs.name_),
          aperture_(rhs.aperture_),
          length_(rhs.length_),
          magnetic_strength_(rhs.magnetic_strength_),
          pos_(rhs.pos_),
          angles_(rhs.angles_),
          s_(rhs.s_),
          beta_(rhs.beta_),
          disp_(rhs.disp_),
          rel_pos_(rhs.rel_pos_) {}

    bool Element::operator==(const Element& rhs) const {
      if (type_ != rhs.type_)
        return false;
      if (s_ != rhs.s_)
        return false;
      if (pos_ != rhs.pos_)
        return false;
      if (magnetic_strength_ != rhs.magnetic_strength_)
        return false;
      if (length_ != rhs.length_)
        return false;
      if (name_ != rhs.name_)
        return false;
      if (aperture_) {
        if (!rhs.aperture_)
          return false;
        if (*aperture_ != *rhs.aperture_)
          return false;
      }
      return true;
    }

    void Element::setAperture(const aperture::AperturePtr& apert) { aperture_ = apert; }

    void Element::setAperture(aperture::Aperture* apert) { setAperture(aperture::AperturePtr(apert)); }

    double Element::fieldStrength(double e_loss, double mp, int qp) const {
      // only act on charged particles
      if (qp == 0)
        return 0.;

      if (e_loss < 0.)
        throw H_ERROR << "Invalid energy loss: " << e_loss << " GeV.";

      double p_bal = 1.;
      if (e_loss > 0.) {
        const double e_ini = Parameters::get().beamEnergy(), mp0 = Parameters::get().beamParticlesMass(),
                     e_out = e_ini - e_loss;
        const double p_ini = sqrt((e_ini - mp0) * (e_ini + mp0)),  // e_ini^2 - p_ini^2 = mp0^2
            p_out = sqrt((e_out - mp) * (e_out + mp));             // e_out^2 - p_out^2 = mp^2

        if (p_out == 0)
          throw H_WARNING << "Invalid particle momentum.";

        p_bal = p_ini / p_out;
      }

      // reweight the field strength by the particle charge and momentum
      return magnetic_strength_ * p_bal * (qp / Parameters::get().beamParticlesCharge());
    }

    const std::string Element::typeName() const {
      std::ostringstream os;
      os << type_;
      return os.str();
    }
  }  // namespace element

  /// Human-readable printout of a beamline element object
  std::ostream& operator<<(std::ostream& os, const element::Element& elem) {
    os << format("%-15s %17s (length = %5.2f m) at %6.2f < s < %6.2f m",
                 elem.typeName().c_str(),
                 elem.name().c_str(),
                 elem.length(),
                 elem.s(),
                 elem.s() + elem.length());
    if (elem.aperture())
      return os << " with aperture: " << elem.aperture();
    return os << " with no aperture restriction";
  }

  /// Human-readable printout of a beamline element object
  std::ostream& operator<<(std::ostream& os, const element::Element* elem) { return os << *elem; }
}  // namespace hector
