#ifndef Elements_RectangularDipole_h
#define Elements_RectangularDipole_h

#include "Dipole.h"

namespace Element
{
  class RectangularDipole : public Dipole
  {
    public:
      RectangularDipole( const char* name ) : Dipole( ElementBase::RectangularDipole, name ) {;}
      ~RectangularDipole() {;}

    private:
  };
}

#endif