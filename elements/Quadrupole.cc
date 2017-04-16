#include "Quadrupole.h"

namespace Hector
{
  namespace Element
  {
    CLHEP::HepMatrix
    HorizontalQuadrupole::matrix( float eloss, float mp, int qp ) const
    {
      const float ke = fabs( fieldStrength( eloss, mp, qp ) );
      if ( ke==0. ) { return Drift::genericMatrix( length_ ); } // simple drift matrix

      const float omega = sqrt( ke )*length_;

      CLHEP::HepMatrix mat = CLHEP::HepDiagMatrix( 6, 1 );
      // Focussing Twiss matrix
      mat( 1, 1 ) = cos( omega );
      mat( 2, 1 ) = sin( omega ) * ( -sqrt( ke ) );
      mat( 1, 2 ) = sin( omega ) * ( 1./sqrt( ke ) );
      mat( 2, 2 ) = cos( omega );
      // Defocussing Twiss matrix
      mat( 3, 3 ) = cosh( omega );
      mat( 4, 3 ) = sinh( omega ) * sqrt( ke );
      mat( 3, 4 ) = sinh( omega ) * ( 1./sqrt( ke ) );
      mat( 4, 4 ) = cosh( omega );
      return mat;
    }

    CLHEP::HepMatrix
    VerticalQuadrupole::matrix( float eloss, float mp, int qp ) const
    {
      const float ke = fabs( fieldStrength( eloss, mp, qp ) );
      if ( ke==0. ) { return Drift::genericMatrix( length_ ); } // simple drift matrix

      const float omega = sqrt( ke )*length_;

      CLHEP::HepMatrix mat = CLHEP::HepDiagMatrix( 6, 1 );
      // Defocussing Twiss matrix
      mat( 1, 1 ) = cosh( omega );
      mat( 2, 1 ) = sinh( omega ) * sqrt( ke );
      mat( 1, 2 ) = sinh( omega ) * ( 1./sqrt( ke ) );
      mat( 2, 2 ) = cosh( omega );
      // Focussing Twiss matrix
      mat( 3, 3 ) = cos( omega );
      mat( 4, 3 ) = sin( omega ) * ( -sqrt( ke ) );
      mat( 3, 4 ) = sin( omega ) * ( 1./sqrt( ke ) );
      mat( 4, 4 ) = cos( omega );
      return mat;
    }
  }
}
