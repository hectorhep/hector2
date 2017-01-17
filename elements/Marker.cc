#include "Marker.h"

namespace Hector
{
  namespace Element
  {
    Marker::Marker( const std::string& name ) : ElementBase( ElementBase::Marker, name )
    {}

    CLHEP::HepMatrix
    Marker::matrix( float, float, int ) const
    {
      return CLHEP::HepDiagMatrix( 6, 1 );
    }
  }
}
