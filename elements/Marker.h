#ifndef Elements_Marker_h
#define Elements_Marker_h

#include "ElementBase.h"

namespace Element
{
  class Marker : public ElementBase
  {
    public:
      Marker( const std::string& name );

      Marker* clone() const { return new Marker( *this ); }
      CLHEP::HepMatrix matrix( float, float, int ) const;

    private:

  };
}

#endif
