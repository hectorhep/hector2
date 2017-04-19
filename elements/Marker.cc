#include "Marker.h"

namespace Hector
{
  namespace Element
  {
    Marker::Marker( const std::string& name, float spos, float length ) :
      ElementBase( aMarker, name, spos, length )
    {}

    CLHEP::HepMatrix
    Marker::matrix( float, float, int ) const
    {
      return Drift::genericMatrix( length_ );
    }
  }
}
