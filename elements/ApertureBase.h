#ifndef Hector_Elements_Aperture_h
#define Hector_Elements_Aperture_h

#include "core/Utils.h"
#include <CLHEP/Vector/TwoVector.h>
#include <vector>

namespace Hector
{
  /// Collection of apertures to be associated to element objects
  namespace Aperture
  {
    /// List of types allowed for an aperture
    typedef enum {
      anInvalidType,
      aRectangularAperture,
      anEllipticAperture,
      aCircularAperture,
      aRectEllipticAperture
    } Type;
    /// Human-readable printout of an aperture type
    std::ostream& operator<<( std::ostream&, const Type& );

    /// A generic aperture object for a beamline element
    class ApertureBase
    {
      public:
        /// Collection of shape parameters for this aperture
        typedef std::vector<float> Parameters;

      public:
        /// Build a new aperture to an element
        /// \param[in] type Aperture type (see Aperture::ApertureBase::Type)
        /// \param[in] pos Aperture position
        /// \param[in] param Shape parameters
        ApertureBase( const Type& type, const CLHEP::Hep2Vector& pos, const std::vector<float>& param );
        virtual ~ApertureBase();

        /// Return a pointer to a clone of the current aperture
        virtual ApertureBase* clone() const = 0;
        /// Check if two apertures (and their properties) are identical
        bool operator==( const ApertureBase& ) const;
        /// Check if two apertures (and their properties) are different
        bool operator!=( const ApertureBase& rhs ) const { return !( *this==rhs ); }

        /// Check if a position is contained in the aperture
        virtual bool contains( const CLHEP::Hep2Vector& ) const = 0;

        /// Human-readable printout of the properties of an aperture
        friend std::ostream& operator<<( std::ostream&, const ApertureBase& );
        /// Human-readable printout of the properties of an aperture
        friend std::ostream& operator<<( std::ostream&, const ApertureBase* );

        /// Type of aperture (rectangular, elliptic, rect-elliptic, circular)
        Type type() const { return type_; }

        /// Retrieve a shape parameter of the aperture
        float p( const size_t& i ) const { return ( i<param_.size() ) ? param_.at( i ) : -1.0; }

        /// Set the transverse position of the aperture
        void setPosition( const CLHEP::Hep2Vector& pos ) { pos_ = pos; }
        /// Set the transverse position of the aperture
        void setPosition( float x, float y ) { setPosition( CLHEP::Hep2Vector( x, y ) ); }

        /// Get the transverse position of the aperture
        CLHEP::Hep2Vector position() const { return pos_; }
        /// Get the horizontal position of the aperture
        float x() const { return pos_.x(); }
        /// Get the vertical position of the aperture
        float y() const { return pos_.y(); }

      protected:
        /// Type of aperture
        Type type_;
        /// Transverse position of the aperture
        CLHEP::Hep2Vector pos_;
        /// Aperture shape parameters
        Parameters param_;
    };
  }
}

#endif
