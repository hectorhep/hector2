#ifndef Elements_VerticalQuadrupole_h
#define Elements_VerticalQuadrupole_h

#include "Quadrupole.h"

namespace Element
{
  class VerticalQuadrupole : public Quadrupole
  {
    public:
      VerticalQuadrupole( const char* name ) : Quadrupole( name ) {;}
      ~VerticalQuadrupole() {;}

    private:
  };
}

#endif
