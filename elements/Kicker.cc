#include "Kicker.h"

namespace Hector
{
  namespace Element
  {
    CLHEP::HepMatrix
    HorizontalKicker::matrix( float eloss, float mp, int qp ) const
    {
      const float ke = -fieldStrength( eloss, mp, qp );

      CLHEP::HepMatrix mat = CLHEP::HepDiagMatrix( 6, 1 );
      //if ( ke==0. ) return mat; // FIXME identity or drift matrix?
      if ( ke==0 ) return Drift::genericMatrix( length_ );

      mat( 1, 2 ) = length_;
      mat( 3, 4 ) = length_;
      mat( 1, 6 ) = length_*tan( ke ) / 2.;
      mat( 2, 6 ) = ke;
      return mat;
    }

    CLHEP::HepMatrix
    VerticalKicker::matrix( float eloss, float mp, int qp ) const
    {
      const float ke = -fieldStrength( eloss, mp, qp );

      CLHEP::HepMatrix mat = CLHEP::HepDiagMatrix( 6, 1 );
      //if ( ke==0. ) return mat; // FIXME identity or drift matrix?
      if ( ke==0 ) return Drift::genericMatrix( length_ );

      mat( 1, 2 ) = length_;
      mat( 3, 4 ) = length_;
      mat( 3, 6 ) = length_*tan( ke ) / 2.;
      mat( 4, 6 ) = ke;
      return mat;
    }
  }
}
