#ifndef Hector_Elements_ElementBase_h
#define Hector_Elements_ElementBase_h

#include "ApertureBase.h"
#include "ElementType.h"

#include <CLHEP/Matrix/Matrix.h>
#include <memory>

namespace Hector
{
  /// Collection of beamline elements
  namespace Element
  {
    /// A generic beamline element object
    class ElementBase
    {
      public:
        /// Build a new element
        /// \param[in] type Element type (see Element::Type)
        /// \param[in] name Element name
        /// \param[in] spos s-position of the element in the beamline
        /// \param[in] length Element length (in m)
        ElementBase( const Type& type, const std::string& name="invalid element", float spos = 0., float length = 0. );
        /// Copy constructor (moving the associated aperture if any)
        ElementBase( ElementBase& elem );
        /// Copy constructor (cloning the associated aperture if any)
        ElementBase( const ElementBase& elem );
        virtual ~ElementBase() {}

        /// Return a pointer to a clone of the current element
        virtual std::shared_ptr<ElementBase> clone() const = 0;
        /// Check if two elements (and their properties) are identical
        bool operator==( const ElementBase& ) const;
        /// Check if two elements (and their properties) are different
        bool operator!=( const ElementBase& rhs ) const { return !( *this == rhs ); }

        /// Compute the propagation matrix for this element
        /// \param[in] eloss Particle energy loss in the element (GeV)
        /// \param[in] mp Particle mass (GeV)
        /// \param[in] qp Particle charge (e)
        virtual CLHEP::HepMatrix matrix( float eloss, float mp, int qp ) const = 0;

        /// Human-readable printout of the properties of an element
        friend std::ostream& operator<<( std::ostream&, const ElementBase& );
        /// Human-readable printout of the properties of an element
        friend std::ostream& operator<<( std::ostream&, const ElementBase* );

        /// Set the name of the element
        void setName( const std::string& name ) { name_ = name; }
        /// Element name
        const std::string& name() const { return name_; }
        /// Set the element type
        void setType( const Type& type ) { type_ = type; }
        /// Element type
        Type type() const { return type_; }

        /// Set the longitudinal position of the entrance of the element
        void setS( float s ) { s_ = s; }
        /// Longitudinal position
        float s() const { return s_; }

        /// Set the x-y position of the centre of the element
        void setPosition( const CLHEP::Hep2Vector& pos ) { pos_ = pos; }
        /// Change the x-y position of the element
        void offset( const CLHEP::Hep2Vector& offset ) {
          pos_ += offset;
          if ( aperture_ ) aperture_->offset( offset );
        }
        /// x-y position of the element at a given s
        CLHEP::Hep2Vector position() const { return pos_; }
        /// Horizontal position
        float x() const { return pos_.x(); }
        /// Vertical position
        float y() const { return pos_.y(); }

        /// Set the horizontal and vertical angles of the element (computed with respect to the s coordinate)
        void setAngles( const CLHEP::Hep2Vector& angles ) { angles_ = angles; }
        /// Change the orientation of the element
        void tilt( const CLHEP::Hep2Vector& tilt ) { angles_ += tilt; }
        /// Horizontal and vertical tilts of the element (with respect to the s axis)
        CLHEP::Hep2Vector angles() const { return angles_; }
        /// Horizontal angle
        float Tx() const { return angles_.x(); }
        /// Vertical angle
        float Ty() const { return angles_.y(); }

        /// Set the element length (m)
        void setLength( float length ) { length_ = length; }
        /// Element length (m)
        float length() const { return length_; }

        /// Set the element magnetic field strength
        /// \note Strength \f$ k = \frac{e}{p}\frac{\partial B}{\partial x} \f$
        void setMagneticStrength( float k ) { magnetic_strength_ = k; }
        /// Magnetic field strength
        float magneticStrength() const { return magnetic_strength_; }

        /// Set the beta factor
        void setBeta( const CLHEP::Hep2Vector& beta ) { beta_ = beta; }
        /// Beta factor
        CLHEP::Hep2Vector beta() const { return beta_; }

        /// Set the x-y (horizontal and vertical) dispersions
        void setDispersion( const CLHEP::Hep2Vector& disp ) { disp_ = disp; }
        /// Horizontal and vertical dispersion
        CLHEP::Hep2Vector dispersion() const { return disp_; }

        /// Set the relative position of the element
        void setRelativePosition( const CLHEP::Hep2Vector& pos ) { rel_pos_ = pos; };
        /// Relative position of the element
        CLHEP::Hep2Vector relativePosition() const { return rel_pos_; }

        /// Set the aperture for this element
        void setAperture( const std::shared_ptr<Aperture::ApertureBase> apert );
        /// Aperture
        Aperture::ApertureBase* aperture() const { return aperture_.get(); }

        /// Compute the modified field strength of the element for a given energy loss of a particle of given mass and charge
        /// \note \f$ k_e = k \cdot \frac{p}{p-\mathrm{d}p} \cdot \frac{q_{\mathrm{part}}}{q_{\mathrm{b}}} \f$
        float fieldStrength( float, float, int ) const;

      protected:
        /// Element type
        Type type_;
        /// Element name
        std::string name_;
        /// Pointer to the associated aperture object (if any)
        std::shared_ptr<Aperture::ApertureBase> aperture_;

        /// Element longitudinal length
        float length_;
        /// Magnetic strength of the object
        float magnetic_strength_;

        /// x-y position of the element
        CLHEP::Hep2Vector pos_;
        /// Horizontal and vertical angles of the element
        CLHEP::Hep2Vector angles_;
        /// Longitudinal position of the element
        float s_;

        /// Beta factor
        CLHEP::Hep2Vector beta_;
        /// Dispersion
        CLHEP::Hep2Vector disp_;
        /// Relative position of the element
        CLHEP::Hep2Vector rel_pos_;
    };

    /// Sorting methods for the beamline construction (using the s position of each elements)
    struct ElementsSorter
    {
      /// Compare the references to two elements
      inline bool operator()( const ElementBase& lhs, const ElementBase& rhs ) const {
        return ( &lhs < &rhs );
      }
      /// Compare the pointers to two elements
      inline bool operator()( const std::shared_ptr<ElementBase> lhs, const std::shared_ptr<ElementBase> rhs ) const {
        if ( lhs->s() < rhs->s() ) return true;
        if ( lhs->s() > rhs->s() ) return false;
        if ( lhs->s()+lhs->length() < rhs->s()+rhs->length() ) return true;
        return false;
      }
    };
  }
  /// List of elements
  typedef std::vector<std::shared_ptr<Element::ElementBase> > Elements;
}

#endif
