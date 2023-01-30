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

#ifndef Hector_Elements_Element_h
#define Hector_Elements_Element_h

#include <iosfwd>
#include <memory>

#include "Hector/Apertures/Aperture.h"
#include "Hector/Elements/ElementFwd.h"
#include "Hector/Elements/ElementType.h"
#include "Hector/Utils/Algebra.h"

namespace hector {
  /// Collection of beamline elements
  namespace element {
    /// A generic beamline element object
    class Element {
    public:
      /// Build a new element
      /// \param[in] type Element type (see element::Type)
      /// \param[in] name Element name
      /// \param[in] spos s-position of the element in the beamline
      /// \param[in] length Element length (in m)
      explicit Element(const Type& type,
                       const std::string& name = "invalid element",
                       double spos = 0.,
                       double length = 0.);
      /// Copy constructor (moving the associated aperture if any)
      Element(Element& elem);
      /// Copy constructor (cloning the associated aperture if any)
      Element(const Element& elem);
      virtual ~Element() {}

      /// Return a pointer to a clone of the current element
      virtual ElementPtr clone() const = 0;
      /// Check if two elements (and their properties) are identical
      bool operator==(const Element&) const;
      /// Check if two elements (and their properties) are different
      bool operator!=(const Element& rhs) const { return !(*this == rhs); }

      /// Compute the propagation matrix for this element
      /// \param[in] eloss Particle energy loss in the element (GeV)
      /// \param[in] mp Particle mass (GeV)
      /// \param[in] qp Particle charge (e)
      virtual Matrix matrix(double eloss, double mp = -1., int qp = 0) const = 0;

      /// Set the name of the element
      void setName(const std::string& name) { name_ = name; }
      /// Element name
      const std::string name() const { return name_; }
      /// Set the element type
      void setType(const Type& type) { type_ = type; }
      /// Element type
      Type type() const { return type_; }
      /// Human-readable element type
      const std::string typeName() const;

      /// Set the longitudinal position of the entrance of the element
      void setS(double s) { s_ = s; }
      /// Longitudinal position
      double s() const { return s_; }
      /// Offset the longitudinal position by a given distance (in m)
      void offsetS(double s_offs) { s_ += s_offs; }

      /// Set the x-y position of the centre of the element
      void setPosition(const TwoVector& pos) { pos_ = pos; }
      /// Change the x-y position of the element
      void offset(const TwoVector& offset) {
        pos_ += offset;
        if (aperture_)
          aperture_->offset(offset);
      }
      /// x-y position of the element at a given s
      TwoVector position() const { return pos_; }
      /// Horizontal position
      double x() const { return pos_.x(); }
      /// Vertical position
      double y() const { return pos_.y(); }

      /// Set the horizontal and vertical angles of the element (computed with respect to the s coordinate)
      void setAngles(const TwoVector& angles) { angles_ = angles; }
      /// Change the orientation of the element
      void tilt(const TwoVector& tilt) { angles_ += tilt; }
      /// Horizontal and vertical tilts of the element (with respect to the s axis)
      TwoVector angles() const { return angles_; }
      /// Horizontal angle
      double Tx() const { return angles_.x(); }
      /// Vertical angle
      double Ty() const { return angles_.y(); }

      /// Set the element length (m)
      void setLength(double length) { length_ = length; }
      /// Element length (m)
      double length() const { return length_; }

      /// Set the element magnetic field strength
      /// \note Strength \f$ k = \frac{e}{p}\frac{\partial B}{\partial x} \f$
      void setMagneticStrength(double k) { magnetic_strength_ = k; }
      /// Magnetic field strength
      double magneticStrength() const { return magnetic_strength_; }

      /// Set the beta factor
      void setBeta(const TwoVector& beta) { beta_ = beta; }
      /// Beta factor
      TwoVector beta() const { return beta_; }

      /// Set the x-y (horizontal and vertical) dispersions
      void setDispersion(const TwoVector& disp) { disp_ = disp; }
      /// Horizontal and vertical dispersions
      TwoVector dispersion() const { return disp_; }

      /// Set the relative position of the element
      void setRelativePosition(const TwoVector& pos) { rel_pos_ = pos; };
      /// Relative position of the element
      TwoVector relativePosition() const { return rel_pos_; }

      /// Set the aperture for this element
      void setAperture(aperture::Aperture* apert);
      /// Set the aperture for this element
      void setAperture(const aperture::AperturePtr& apert);
      /// Aperture
      aperture::Aperture* aperture() const { return aperture_.get(); }

      /// Set the parent element if this one is splitted
      void setParentElement(const ElementPtr& parent) { parent_elem_ = parent; }
      /// Parent element if this one is splitted
      Element* parentElement() const { return parent_elem_.get(); }

      /// Compute the modified field strength of the element for a given energy loss of a particle of given mass and charge
      /// \note \f$ k_e = k \cdot \frac{p}{p-\mathrm{d}p} \cdot \frac{q_{\mathrm{part}}}{q_{\mathrm{b}}} \f$
      double fieldStrength(double, double = -1., int = 0) const;

    protected:
      /// Element type
      Type type_;
      /// Element name
      std::string name_;
      /// Pointer to the associated aperture object (if any)
      aperture::AperturePtr aperture_;
      /// Pointer to the parent element (if divided)
      ElementPtr parent_elem_;

      /// Element longitudinal length
      double length_;
      /// Magnetic strength of the object
      double magnetic_strength_;

      /// x-y position of the element
      TwoVector pos_;
      /// Horizontal and vertical angles of the element
      TwoVector angles_;
      /// Longitudinal position of the element
      double s_;

      /// Beta factor
      TwoVector beta_;
      /// Dispersion
      TwoVector disp_;
      /// Relative position of the element
      TwoVector rel_pos_;
    };

    /// Sorting methods for the beamline construction (using the s position of each elements)
    struct ElementsSorter {
      /// Compare the pointers to two elements
      inline bool operator()(const Element* lhs, const Element* rhs) const {
        if (!lhs || !rhs)
          return false;
        if (lhs->s() < rhs->s())
          return true;
        if (lhs->s() + lhs->length() < rhs->s() + rhs->length())
          return true;
        return false;
      }
      /// Compare the references to two elements
      inline bool operator()(const Element& lhs, const Element& rhs) const { return (&lhs < &rhs); }
      /// Compare the smart pointers to two elements
      inline bool operator()(const ElementPtr& lhs, const ElementPtr& rhs) const {
        if (lhs->s() < rhs->s())
          return true;
        if (lhs->s() + lhs->length() < rhs->s() + rhs->length())
          return true;
        return false;
      }
    };
  }  // namespace element
  /// Human-readable printout of the properties of an element
  std::ostream& operator<<(std::ostream&, const element::Element&);
  /// Human-readable printout of the properties of an element
  std::ostream& operator<<(std::ostream&, const element::Element*);
}  // namespace hector

#endif
