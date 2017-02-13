#ifndef Elements_ElementBase_h
#define Elements_ElementBase_h

#include "ApertureBase.h"
#include "core/Utils.h"
#include "core/Constants.h"

#include <sstream>

#include <CLHEP/Vector/TwoVector.h>
#include <CLHEP/Matrix/Matrix.h>
#include <CLHEP/Matrix/DiagMatrix.h>

namespace Hector
{
  /// Collection of beamline elements
  namespace Element
  {
    /// A generic beamline element object
    class ElementBase
    {
      public:
        /// List of types allowed for an element
        typedef enum {
          Invalid = -1, ///< Invalid element
          Marker, ///< Simple s-marker
          Drift, ///< Drift element
          Monitor, ///< Monitoring element
          RectangularDipole, ///< Rectangular-type dipole
          SectorDipole, ///< Sector-type dipole
          Quadrupole, ///< Generic (vertical or horizontal) quadrupole
          Sextupole, ///< Generic sextupole
          Multipole, ///< Generic multipole
          VerticalKicker, ///< Vertical-type kicker
          HorizontalKicker, ///< Horizontal-type kicker
          RectangularCollimator, ///< Rectangular-type collimator
          EllipticalCollimator, ///< Rectangular-type collimator
          CircularCollimator, ///< Circular-type collimator
          //RomanPot, InteractionPoint,
          Placeholder, ///< A placeholder element
          Instrument ///< A measurement apparatus
        } Type;
        /// Human-readable printout of a element type
        friend std::ostream& operator<<( std::ostream&, const Type& );

      public:
        /// Build a new element
        /// \param[in] type Element type (see Element::ElementBase::Type)
        /// \param[in] name Element name
        ElementBase( const Type& type, const std::string& name="invalid element" );
        virtual ~ElementBase();

        /// Return a pointer to a clone of the current element
        virtual ElementBase* clone() const = 0;
        /// Compute the propagation matrix
        /// \param[in] eloss Particle energy loss in the element (GeV)
        /// \param[in] mp Particle mass (GeV)
        /// \param[in] qp Particle charge (e)
        virtual CLHEP::HepMatrix matrix( float eloss, float mp, int qp ) const = 0;

        /// Human-readable printout of the properties of an element
        friend std::ostream& operator<<( std::ostream&, const ElementBase& );
        /// Human-readable printout of the properties of an element
        friend std::ostream& operator<<( std::ostream&, const ElementBase* );

        /// Element name
        const std::string& name() const { return name_; }
        /// Element type
        Type type() const { return type_; }

        /// Set the longitudinal position of the entrance of the element
        void setS( float s ) { s_ = s; }
        /// Longitudinal position
        float s() const { return s_; }

        /// Set the x-y position of the centre of the element
        void setPosition( const CLHEP::Hep2Vector& pos ) { pos_ = pos; }
        /// x-y position of the element at a given s
        CLHEP::Hep2Vector position() const { return pos_; }
        /// Horizontal position
        float x() const { return pos_.x(); }
        /// Vertical position
        float y() const { return pos_.y(); }

        /// Set the horizontal and vertical angles of the element (computed with respect to the s coordinate)
        void setAngles( const CLHEP::Hep2Vector& angles ) { angles_ = angles; }
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
        void setAperture( const Aperture::ApertureBase* apert, bool delete_after=false ) {
          aperture_ = apert->clone();
          if ( delete_after ) delete apert;
        }
        /// Aperture
        Aperture::ApertureBase* aperture() const { return aperture_; }

        /// Compute the modified field strength of the element for a given energy loss of a particle of given mass and charge
        /// \note \f$ k_e = k \cdot \frac{p}{p-\mathrm{d}p} \cdot \frac{q_{\mathrm{part}}}{q_{\mathrm{b}}} \f$
        float fieldStrength( float, float, int ) const;

      protected:
        /// Element type
        Type type_;
        /// Element name
        const std::string name_;
        /// Pointer to the associated aperture object (if any)
        Aperture::ApertureBase* aperture_;

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
      inline bool operator()( const ElementBase& lhs, const ElementBase& rhs ) { return ( lhs.s()<rhs.s() ); }
      /// Compare the pointers to two elements
      inline bool operator()( const ElementBase* lhs, const ElementBase* rhs ) { return ( lhs->s()<rhs->s() ); }
    };

  }
}

#endif
