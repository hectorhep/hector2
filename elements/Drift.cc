#include "Drift.h"

namespace Hector
{
  namespace Element
  {
    Drift::Drift( const std::string& name, const ElementBase::Type& type ) :
      ElementBase( type, name )
    {}

    CLHEP::HepMatrix
    Drift::genericMatrix( float length )
    {
      CLHEP::HepMatrix mat = CLHEP::HepDiagMatrix( 6, 1 );
      mat( 2, 1 ) = length;
      mat( 4, 3 ) = length;
      return mat;
    }
  }
}
