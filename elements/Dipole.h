#ifndef Elements_Dipole_h
#define Elements_Dipole_h

#include "ElementBase.h"
#include "core/Exception.h"

namespace Hector
{
  namespace Element
  {
    /// A generic dipole object
    class Dipole : public ElementBase
    {
      public:
        Dipole( const ElementBase::Type& type, const std::string& name ) : ElementBase( type, name ) {}

        virtual Dipole* clone() const = 0;
        virtual CLHEP::HepMatrix matrix( float, float, int ) const = 0;

      protected:
    };

    /// Rectangular dipole object builder
    class RectangularDipole : public Dipole
    {
      public:
        RectangularDipole( const std::string& name ) : Dipole( ElementBase::RectangularDipole, name ) {}

        RectangularDipole* clone() const {
          RectangularDipole* out = new RectangularDipole( *this );
          if ( aperture_ ) out->setAperture( aperture_->clone() );
          return out;
        }
        CLHEP::HepMatrix matrix( float, float, int ) const;

      private:
    };

    /// Sector dipole object builder
    class SectorDipole : public Dipole
    {
      public:
        SectorDipole( const std::string& name ) : Dipole( ElementBase::SectorDipole, name ) {}

        SectorDipole* clone() const {
          SectorDipole* out = new SectorDipole( *this );
          if ( aperture_ ) out->setAperture( aperture_->clone() );
          return out;
        }
        CLHEP::HepMatrix matrix( float, float, int ) const;

      private:
    };
  }
}

#endif
