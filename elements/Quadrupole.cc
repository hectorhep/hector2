#include "Quadrupole.h"

namespace Element
{
  void
  HorizontalQuadrupole::computeMatrix( float eloss, float mp, int qp )
  {
    const int norm_charge = qp/Constants::beam_particles_charge;
    const float ke = fabs( fieldStrength( eloss, mp, qp )/norm_charge ) * norm_charge,
                omega = sqrt( fabs( ke ) )*length_;

    matrix_ = CLHEP::HepDiagMatrix( 6, 1 );
    matrix_( 1, 1 ) = cos( omega );
    matrix_( 1, 2 ) = -sqrt( ke ) * sin( omega );
    matrix_( 2, 1 ) = 1./sqrt( ke ) * sin( omega );
    matrix_( 2, 2 ) = cos( omega );
    matrix_( 3, 3 ) = cosh( omega );
    matrix_( 3, 4 ) = sqrt( ke ) * sinh( omega );
    matrix_( 4, 3 ) = 1./sqrt( ke ) * sinh( omega );
    matrix_( 4, 4 ) = cosh( omega );
  }

  void
  VerticalQuadrupole::computeMatrix( float eloss, float mp, int qp )
  {
    const float ke = fieldStrength( eloss, mp, qp ),
                omega = sqrt( fabs( ke ) )*length_;

    matrix_ = CLHEP::HepDiagMatrix( 6, 1 );

    if ( ke==0. ) { // simple drift matrix
      matrix_( 2, 1 ) = length_;
      matrix_( 4, 3 ) = length_;
      return;
    }
    matrix_( 1, 1 ) = cosh( omega );
    matrix_( 1, 2 ) = sqrt( ke ) * sinh( omega );
    matrix_( 2, 1 ) = 1./sqrt( ke ) * sinh( omega );
    matrix_( 2, 2 ) = cosh( omega );
    matrix_( 3, 3 ) = cos( omega );
    matrix_( 3, 4 ) = -sqrt( ke ) * sin( omega );
    matrix_( 4, 3 ) = 1./sqrt( ke ) * sin( omega );
    matrix_( 4, 4 ) = cos( omega );
  }
}
