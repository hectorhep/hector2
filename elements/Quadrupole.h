#ifndef Elements_Quadrupole_h
#define Elements_Quadrupole_h

#include "ElementBase.h"

namespace Element
{
  class Quadrupole : public ElementBase
  {
    public:
      Quadrupole( const std::string& name ) : ElementBase( ElementBase::Quadrupole, name ) {;}
      ~Quadrupole() {;}

    protected:
  };

  class HorizontalQuadrupole : public Quadrupole
  {
    public:
      HorizontalQuadrupole( const std::string& name ) : Quadrupole( name ) {;}
      ~HorizontalQuadrupole() {;}

    private:
  };

  class VerticalQuadrupole : public Quadrupole
  {
    public:
      VerticalQuadrupole( const std::string& name ) : Quadrupole( name ) {;}
      ~VerticalQuadrupole() {;}

    private:
  };
}

#endif
