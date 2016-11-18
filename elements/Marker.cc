#include "Marker.h"

namespace Element
{
  Marker::Marker( const std::string& name ) : ElementBase( ElementBase::Marker, name )
  {
  }

  void
  Marker::computeMatrix( float, float, int )
  {
    matrix_ = CLHEP::HepDiagMatrix( 6, 1 );
  }
}
