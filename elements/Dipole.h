#ifndef Elements_Dipole_h
#define Elements_Dipole_h

#include "ElementBase.h"

namespace Element
{
  class Dipole : public ElementBase
  {
    public:
      Dipole( const ElementBase::Type& type, const std::string& name ) : ElementBase( type, name ) {;}
      ~Dipole() {;}

    protected:
  };

  class RectangularDipole : public Dipole
  {
    public:
      RectangularDipole( const std::string& name ) : Dipole( ElementBase::RectangularDipole, name ) {;}
      ~RectangularDipole() {;}

    private:
  };

  class SectorDipole : public Dipole
  {
    public:
      SectorDipole( const std::string& name ) : Dipole( ElementBase::SectorDipole, name ) {;}
      ~SectorDipole() {;}

    private:
  };
}

#endif
