#ifndef Hector_Elements_ApertureBase_h
#define Hector_Elements_ApertureBase_h

#include "Hector/Core/Algebra.h"
#include "Hector/Elements/ApertureType.h"

#include <vector>
#include <memory>

using std::ostream;

namespace Hector
{
  /// Collection of apertures to be associated to element objects
  namespace Aperture
  {
    /// A generic aperture object for a beamline element
    class ApertureBase
    {
      public:
        /// Collection of shape parameters for this aperture
        typedef std::vector<float> Parameters;

      public:
        /// Build a new aperture to an element
        /// \param[in] type Aperture type (see Aperture::Type)
        /// \param[in] pos Aperture position
        /// \param[in] param Shape parameters
        ApertureBase( const Type& type, const TwoVector& pos, const std::vector<float>& param );
        virtual ~ApertureBase();

        /// Return a pointer to a clone of the current aperture
        virtual std::shared_ptr<ApertureBase> clone() const = 0;
        /// Check if two apertures (and their properties) are identical
        bool operator==( const ApertureBase& ) const;
        /// Check if two apertures (and their properties) are different
        bool operator!=( const ApertureBase& rhs ) const { return !( *this==rhs ); }

        /// Check if a position is contained in the aperture
        virtual bool contains( const TwoVector& ) const = 0;
        /// Get the outer boundaries of the aperture
        virtual TwoVector limits() const = 0;

        /// Type of aperture (rectangular, elliptic, rect-elliptic, circular)
        Type type() const { return type_; }

        /// Retrieve a shape parameter of the aperture
        float p( const size_t& i ) const { return ( i<param_.size() ) ? param_.at( i ) : -1.0; }
        const Parameters& parameters() const { return param_; }

        /// Set the transverse position of the aperture barycentre
        void setPosition( const TwoVector& pos ) { pos_ = pos; }
        /// Set the transverse position of the aperture barycentre
        void setPosition( float x, float y ) { setPosition( TwoVector( x, y ) ); }
        /// Change the x-y position of the aperture barycentre
        void offset( const TwoVector& offs ) { pos_ += offs; }

        /// Get the transverse position of the aperture barycentre
        TwoVector position() const { return pos_; }
        /// Get the horizontal position of the aperture barycentre
        float x() const { return pos_.x(); }
        /// Get the vertical position of the aperture barycentre
        float y() const { return pos_.y(); }

      protected:
        /// Type of aperture
        Type type_;
        /// Transverse position of the aperture
        TwoVector pos_;
        /// Aperture shape parameters
        Parameters param_;
    };
  }
  /// Human-readable printout of the properties of an aperture
  std::ostream& operator<<( std::ostream&, const Aperture::ApertureBase& );
  /// Human-readable printout of the properties of an aperture
  std::ostream& operator<<( std::ostream&, const Aperture::ApertureBase* );
}

#endif
