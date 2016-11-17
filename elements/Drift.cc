#include "Drift.h"

namespace Element
{
  Drift::Drift( const ElementBase::Type& type, const std::string& name ) :
    ElementBase( type, name )
  {
  }

  Drift::~Drift()
  {}

  void
  Drift::computeMatrix( float, float, int )
  {
    matrix_ = CLHEP::HepDiagMatrix( 6, 1 );
    matrix_( 2, 1 ) = length_;
    matrix_( 4, 3 ) = length_;
  }
}
