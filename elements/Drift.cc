#include "Drift.h"

namespace Hector
{
  namespace Element
  {
    Drift::Drift( const ElementBase::Type& type, const std::string& name ) :
      ElementBase( type, name )
    {}

    Drift::~Drift()
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
      mat( 2, 1 ) = length;
      mat( 4, 3 ) = length;
      return mat;
    }
  }
}
