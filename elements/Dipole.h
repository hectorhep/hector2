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
}

#endif
