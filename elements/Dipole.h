#ifndef Elements_Dipole_h
#define Elements_Dipole_h

#include "ElementBase.h"
#include "core/Exception.h"

namespace Hector
{
  namespace Element
  {
    class Dipole : public ElementBase
    {
      public:
        Dipole( const ElementBase::Type& type, const std::string& name ) : ElementBase( type, name ) {}

        virtual Dipole* clone() const = 0;
        virtual CLHEP::HepMatrix matrix( float, float, int ) const = 0;

      protected:
    };

    class RectangularDipole : public Dipole
    {
      public:
        RectangularDipole( const std::string& name ) : Dipole( ElementBase::RectangularDipole, name ) {}

        RectangularDipole* clone() const { return new RectangularDipole( *this ); }
        CLHEP::HepMatrix matrix( float, float, int ) const;

      private:
    };

    class SectorDipole : public Dipole
    {
      public:
        SectorDipole( const std::string& name ) : Dipole( ElementBase::SectorDipole, name ) {}

        SectorDipole* clone() const { return new SectorDipole( *this ); }
        CLHEP::HepMatrix matrix( float, float, int ) const;

      private:
    };
  }
}

#endif
