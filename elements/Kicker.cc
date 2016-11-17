#include "Kicker.h"

namespace Element
{
  void
  HorizontalKicker::computeMatrix( float eloss, float mp, int qp )
  {
    const float ke = -fieldStrength( eloss, mp, qp );

    matrix_ = CLHEP::HepDiagMatrix( 6, 1 );
    matrix_( 2, 1 ) = length_;
    matrix_( 4, 3 ) = length_;
    matrix_( 6, 1 ) = length_*tan( -fieldStrength( eloss, mp, qp ) ) / 2.;
    matrix_( 6, 2 ) = ke;
  }

  void
  VerticalKicker::computeMatrix( float eloss, float mp, int qp )
  {
    const float ke = -fieldStrength( eloss, mp, qp );

    matrix_ = CLHEP::HepDiagMatrix( 6, 1 );
    matrix_( 2, 1 ) = length_;
    matrix_( 4, 3 ) = length_;
    matrix_( 6, 3 ) = length_*tan( ke ) / 2.;
    matrix_( 6, 4 ) = ke;
  }
}
