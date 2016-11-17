#include "Dipole.h"

namespace Element
{
  void
  SectorDipole::computeMatrix( float eloss, float mp, int qp )
  {
    const float ke = fieldStrength( eloss, mp, qp );

    matrix_ = CLHEP::HepDiagMatrix( 6, 1 );

    if ( ke==0. ) { // simple drift matrix
      matrix_( 2, 1 ) = length_;
      matrix_( 4, 3 ) = length_;
      return;
    }

    const float radius = 1./ke,
                norm_l = length_/radius;

    matrix_( 1, 1 ) = cos( norm_l );
    matrix_( 1, 2 ) = -1./radius * sin( norm_l );
    matrix_( 2, 1 ) = radius* sin( norm_l );
    matrix_( 2, 2 ) = cos( norm_l );
    matrix_( 4, 3 ) = length_;
    if ( !Constants::use_relative_energy ) {
      const float simp = 2.*radius*pow( sin( length_/( 2.*radius ) ), 2 ) / Constants::beam_energy;
      matrix_( 5, 1 ) = simp;
      matrix_( 5, 2 ) = sin( norm_l ) / Constants::beam_energy;
    }
  }

  void
  RectangularDipole::computeMatrix( float eloss, float mp, int qp )
  {
    const float ke = fieldStrength( eloss, mp, qp );

    matrix_ = CLHEP::HepDiagMatrix( 6, 1 );

    if ( ke==0. ) { // simple drift matrix
      matrix_( 2, 1 ) = length_;
      matrix_( 4, 3 ) = length_;
      throw Exception( __PRETTY_FUNCTION__, "Dipole has no effect. Results will be corrupted.", JustWarning );
    }

    const float radius = 1./ke,
                norm_l = length_/radius,
                simp = 2.*radius*pow( sin( length_/( 2.*radius ) ), 2 ) / Constants::beam_energy;

    matrix_( 1, 1 ) = cos( norm_l );
    matrix_( 1, 2 ) = -1./radius * sin( norm_l );
    matrix_( 2, 1 ) = radius * sin( norm_l );
    matrix_( 2, 2 ) = cos( norm_l );
    matrix_( 4, 3 ) = length_;
    matrix_( 5, 1 ) = simp;
    matrix_( 5, 2 ) = sin( norm_l ) / Constants::beam_energy;

    if ( Constants::use_relative_energy ) {
      const float psy = ke*length_/2.;

      CLHEP::HepMatrix ef_matrix = CLHEP::HepDiagMatrix( 6, 1 );
      ef_matrix( 1, 2 ) = tan( psy ) * ke;
      ef_matrix( 3, 4 ) = -tan( psy ) * ke;

      matrix_ = ef_matrix * matrix_ * ef_matrix;
    }
  }
}
