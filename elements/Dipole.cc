#include "Dipole.h"

namespace Hector
{
  namespace Element
  {
    CLHEP::HepMatrix
    SectorDipole::matrix( float eloss, float mp, int qp ) const
    {
      const float ke = fieldStrength( eloss, mp, qp );

      if ( ke==0. ) { // simple drift matrix
        Exception( __PRETTY_FUNCTION__, Form( "Dipole %s has no effect. Results will be corrupted.", name_.c_str() ), JustWarning ).dump();
        return Drift::genericMatrix( length_ );
      }

      const float radius = 1./ke,
                  theta = length_/radius;

      CLHEP::HepMatrix mat = CLHEP::HepDiagMatrix( 6, 1 );

      mat( 1, 1 ) = cos( theta );
      mat( 1, 2 ) = -1./radius * sin( theta );
      mat( 2, 1 ) = radius* sin( theta );
      mat( 2, 2 ) = cos( theta );
      mat( 4, 3 ) = length_;
      if ( Constants::use_relative_energy ) {
        const float simp = 2.*radius*pow( sin( length_/( 2.*radius ) ), 2 ) / Constants::beam_energy;
        mat( 5, 1 ) = simp;
        mat( 5, 2 ) = sin( theta ) / Constants::beam_energy;
      }
      return mat;
    }

    CLHEP::HepMatrix
    RectangularDipole::matrix( float eloss, float mp, int qp ) const
    {
      const float ke = fieldStrength( eloss, mp, qp );

      if ( ke==0. ) { // simple drift matrix
        Exception( __PRETTY_FUNCTION__, Form( "Dipole %s has no effect. Results will be corrupted.", name_.c_str() ), JustWarning ).dump();
        return Drift::genericMatrix( length_ );
      }

      const float radius = 1./ke,
                  theta = length_/radius,
                  simp = 2.*radius*pow( sin( length_/( 2.*radius ) ), 2 ) / Constants::beam_energy;

      CLHEP::HepMatrix mat = CLHEP::HepDiagMatrix( 6, 1 );

      mat( 1, 1 ) = cos( theta );
      mat( 1, 2 ) = -1./radius * sin( theta );
      mat( 2, 1 ) = radius * sin( theta );
      mat( 2, 2 ) = cos( theta );
      mat( 4, 3 ) = length_;
      mat( 5, 1 ) = simp;
      mat( 5, 2 ) = sin( theta ) / Constants::beam_energy;

      if ( Constants::use_relative_energy ) {
        const float psy = ke*length_/2.;

        CLHEP::HepMatrix ef_matrix = CLHEP::HepDiagMatrix( 6, 1 );
        ef_matrix( 1, 2 ) =  tan( psy ) * ke;
        ef_matrix( 3, 4 ) = -tan( psy ) * ke;

        return ef_matrix * mat * ef_matrix;
      }

      return mat;
    }
  }
}
