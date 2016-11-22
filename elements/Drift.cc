#include "Drift.h"

namespace Element
{
  Drift::Drift( const ElementBase::Type& type, const std::string& name ) :
    ElementBase( type, name )
  {
  }

  Drift::~Drift()
  {}

  CLHEP::HepMatrix
  Drift::matrix( float, float, int ) const
  {
    CLHEP::HepMatrix mat = CLHEP::HepDiagMatrix( 6, 1 );
    mat( 2, 1 ) = length_;
    mat( 4, 3 ) = length_;
    return mat;
  }
}
