#ifndef Elements_Quadrupole_h
#define Elements_Quadrupole_h

#include "ElementBase.h"

namespace Hector
{
  namespace Element
  {
    /// A generic quadrupole object
    class Quadrupole : public ElementBase
    {
      public:
        Quadrupole( const std::string& name ) : ElementBase( ElementBase::Quadrupole, name ) {}

        virtual Quadrupole* clone() const = 0;
        virtual CLHEP::HepMatrix matrix( float, float, int ) const = 0;

      protected:
    };

    /// Horizontal quadrupole object builder
    class HorizontalQuadrupole : public Quadrupole
    {
      public:
        HorizontalQuadrupole( const std::string& name ) : Quadrupole( name ) {}

        HorizontalQuadrupole* clone() const {
          HorizontalQuadrupole* out = new HorizontalQuadrupole( *this );
          if ( aperture_ ) out->setAperture( aperture_->clone() );
          return out;
        }
        CLHEP::HepMatrix matrix( float, float, int ) const;

      private:
    };

    /// Vertical quadrupole object builder
    class VerticalQuadrupole : public Quadrupole
    {
      public:
        VerticalQuadrupole( const std::string& name ) : Quadrupole( name ) {}

        VerticalQuadrupole* clone() const {
          VerticalQuadrupole* out = new VerticalQuadrupole( *this );
          if ( aperture_ ) out->setAperture( aperture_->clone() );
          return out;
        }
        CLHEP::HepMatrix matrix( float, float, int ) const;

      private:
    };
  }
}

#endif
