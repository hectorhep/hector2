#ifndef Elements_HorizontalQuadrupole_h
#define Elements_HorizontalQuadrupole_h

#include "Quadrupole.h"

namespace Element
{
  class HorizontalQuadrupole : public Quadrupole
  {
    public:
      HorizontalQuadrupole( const char* name ) : Quadrupole( name ) {;}
      ~HorizontalQuadrupole() {;}

    private:
  };
}

#endif
