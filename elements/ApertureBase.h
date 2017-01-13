#ifndef Elements_Aperture_h
#define Elements_Aperture_h

#include <CLHEP/Vector/TwoVector.h>
#include <vector>

namespace Hector
{
  namespace Aperture
  {
    class ApertureBase
    {
      public:
        /// List of types allowed for an aperture
        typedef enum { None, Rectangular, Elliptic, Circular, RectElliptic } Type;
        friend std::ostream& operator<<( std::ostream&, const Type& );

      public:
        ApertureBase( const Type&, const CLHEP::Hep2Vector&, float, float, float, float );
        virtual ~ApertureBase();

        /// Return a pointer to a clone of the current aperture
        virtual ApertureBase* clone() const = 0;
        /// Check if a position is contained in the aperture
        virtual bool contains( const CLHEP::Hep2Vector& ) const = 0;

        friend std::ostream& operator<<( std::ostream&, const ApertureBase& );
        friend std::ostream& operator<<( std::ostream&, const ApertureBase* );

        Type type() const { return type_; }

        float p( const size_t& i ) const { return ( i<param_.size() ) ? param_.at( i ) : -1.0; }

        void setPosition( const CLHEP::Hep2Vector& pos ) { pos_ = pos; }
        void setPosition( float x, float y ) { setPosition( CLHEP::Hep2Vector( x, y ) ); }
        CLHEP::Hep2Vector position() const { return pos_; }

        float x() const { return pos_.x(); }
        float y() const { return pos_.y(); }

      protected:
        Type type_;
        CLHEP::Hep2Vector pos_;
        /// Aperture shape parameters
        std::vector<float> param_;
    };
  }
}

#endif
