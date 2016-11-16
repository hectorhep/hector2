#ifndef Elements_Quadrupole_h
#define Elements_Quadrupole_h

#include "ElementBase.h"

namespace Element
{
  class Quadrupole : public ElementBase
  {
    public:
      Quadrupole( const char* name ) : ElementBase( ElementBase::Quadrupole, name ) {;}
      ~Quadrupole() {;}

    protected:
  };
}

#endif
