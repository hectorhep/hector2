#ifndef Elements_Aperture_h
#define Elements_Aperture_h

#include "core/Utils.h"
#include <CLHEP/Vector/TwoVector.h>
#include <vector>

namespace Hector
{
  /// Collection of apertures to be associated to element objects
  namespace Aperture
  {
    /// A generic aperture object for a beamline element
    class ApertureBase
    {
      public:
        /// List of types allowed for an aperture
        typedef enum { None, Rectangular, Elliptic, Circular, RectElliptic } Type;
        /// Human-readable printout of an aperture type
        friend std::ostream& operator<<( std::ostream&, const Type& );

      public:
        /// Build a new aperture to an element
        /// \param[in] type Aperture type (see Aperture::ApertureBase::Type)
        /// \param[in] pos Aperture position
        /// \param[in] param Shape parameters
        ApertureBase( const Type& type, const CLHEP::Hep2Vector& pos, const std::vector<float>& param );
        virtual ~ApertureBase();

        /// Return a pointer to a clone of the current aperture
        virtual ApertureBase* clone() const = 0;
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
        std::vector<float> param_;
    };
  }
}

#endif
