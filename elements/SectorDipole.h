#ifndef Elements_SectorDipole_h
#define Elements_SectorDipole_h

#include "Dipole.h"

namespace Element
{
  class SectorDipole : public Dipole
  {
    public:
      SectorDipole( const char* name ) : Dipole( ElementBase::SectorDipole, name ) {;}
      ~SectorDipole() {;}

    private:
  };
}

#endif
