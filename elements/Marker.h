#ifndef Elements_Marker_h
#define Elements_Marker_h

#include "ElementBase.h"

namespace Element
{
  class Marker : public ElementBase
  {
    public:
      Marker( const std::string& name ) : ElementBase( ElementBase::Marker, name ) {;}
      ~Marker() {;}

      Marker* clone() const { return new Marker( *this ); }
      void computeMatrix( float, float, int ) { matrix_ = CLHEP::HepDiagMatrix( 6, 1 ); }

    private:

  };
}

#endif
