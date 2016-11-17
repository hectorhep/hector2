#ifndef Elements_Dipole_h
#define Elements_Dipole_h

#include "ElementBase.h"
#include "core/Exception.h"

namespace Element
{
  class Dipole : public ElementBase
  {
    public:
      Dipole( const ElementBase::Type& type, const std::string& name ) : ElementBase( type, name ) {;}
      ~Dipole() {;}

      virtual Dipole* clone() const = 0;
      virtual void computeMatrix( float, float, int ) = 0;

    protected:
  };

  class RectangularDipole : public Dipole
  {
    public:
      RectangularDipole( const std::string& name ) : Dipole( ElementBase::RectangularDipole, name ) {;}
      ~RectangularDipole() {;}

      RectangularDipole* clone() const { return new RectangularDipole( *this ); }
      void computeMatrix( float, float, int );

    private:
  };

  class SectorDipole : public Dipole
  {
    public:
      SectorDipole( const std::string& name ) : Dipole( ElementBase::SectorDipole, name ) {;}
      ~SectorDipole() {;}

      SectorDipole* clone() const { return new SectorDipole( *this ); }
      void computeMatrix( float, float, int );

    private:
  };
}

#endif
