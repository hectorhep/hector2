#include "Drift.h"

#include <CLHEP/Matrix/DiagMatrix.h>

namespace Hector
{
  namespace Element
  {
    Drift::Drift( const std::string& name, float spos, float length ) :
      ElementBase( aDrift, name, spos, length )
    {}

    Drift::Drift( const std::string& name, const Type& type, float spos, float length ) :
      ElementBase( type, name, spos, length )
    {}

    CLHEP::HepMatrix
    Drift::matrix( float, float, int ) const
    {
      return genericMatrix( length_ );
    }

    CLHEP::HepMatrix
    Drift::genericMatrix( float length )
    {
      CLHEP::HepMatrix mat = CLHEP::HepDiagMatrix( 6, 1 );
      mat( 1, 2 ) = length;
      mat( 3, 4 ) = length;
      return mat;
    }
  }
}
