#include "Dipole.h"

namespace Element
{
  CLHEP::HepMatrix
  SectorDipole::matrix( float eloss, float mp, int qp ) const
  {
    const float ke = fieldStrength( eloss, mp, qp );

    CLHEP::HepMatrix mat = CLHEP::HepDiagMatrix( 6, 1 );

    if ( ke==0. ) { // simple drift matrix
      mat( 2, 1 ) = length_;
      mat( 4, 3 ) = length_;
      return mat;
    }

    const float radius = 1./ke,
                norm_l = length_/radius;

    mat( 1, 1 ) = cos( norm_l );
    mat( 1, 2 ) = -1./radius * sin( norm_l );
    mat( 2, 1 ) = radius* sin( norm_l );
    mat( 2, 2 ) = cos( norm_l );
    mat( 4, 3 ) = length_;
    if ( !Constants::use_relative_energy ) {
      const float simp = 2.*radius*pow( sin( length_/( 2.*radius ) ), 2 ) / Constants::beam_energy;
      mat( 5, 1 ) = simp;
      mat( 5, 2 ) = sin( norm_l ) / Constants::beam_energy;
    }
    return mat;
  }

  CLHEP::HepMatrix
  RectangularDipole::matrix( float eloss, float mp, int qp ) const
  {
    const float ke = fieldStrength( eloss, mp, qp );

    CLHEP::HepMatrix mat = CLHEP::HepDiagMatrix( 6, 1 );

    if ( ke==0. ) { // simple drift matrix
      mat( 2, 1 ) = length_;
      mat( 4, 3 ) = length_;
      Exception( __PRETTY_FUNCTION__, "Dipole has no effect. Results will be corrupted.", JustWarning ).dump();
      return mat;
    }

    const float radius = 1./ke,
                norm_l = length_/radius,
                simp = 2.*radius*pow( sin( length_/( 2.*radius ) ), 2 ) / Constants::beam_energy;

    mat( 1, 1 ) = cos( norm_l );
    mat( 1, 2 ) = -1./radius * sin( norm_l );
    mat( 2, 1 ) = radius * sin( norm_l );
    mat( 2, 2 ) = cos( norm_l );
    mat( 4, 3 ) = length_;
    mat( 5, 1 ) = simp;
    mat( 5, 2 ) = sin( norm_l ) / Constants::beam_energy;

    if ( Constants::use_relative_energy ) {
      const float psy = ke*length_/2.;

      CLHEP::HepMatrix ef_matrix = CLHEP::HepDiagMatrix( 6, 1 );
      ef_matrix( 1, 2 ) = tan( psy ) * ke;
      ef_matrix( 3, 4 ) = -tan( psy ) * ke;

      return ef_matrix * mat * ef_matrix;
    }

    return mat;
  }
}
