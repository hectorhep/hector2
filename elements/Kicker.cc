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

      mat( 2, 1 ) = length_;
      mat( 4, 3 ) = length_;
      mat( 6, 1 ) = length_*tan( ke ) / 2.;
      mat( 6, 2 ) = ke;
      return mat;
    }

    CLHEP::HepMatrix
    VerticalKicker::matrix( float eloss, float mp, int qp ) const
    {
      const float ke = -fieldStrength( eloss, mp, qp );

      CLHEP::HepMatrix mat = CLHEP::HepDiagMatrix( 6, 1 );
      //if ( ke==0. ) return mat; // FIXME identity or drift matrix?
      if ( ke==0 ) return Drift::genericMatrix( length_ );

      mat( 2, 1 ) = length_;
      mat( 4, 3 ) = length_;
      mat( 6, 3 ) = length_*tan( ke ) / 2.;
      mat( 6, 4 ) = ke;
      return mat;
    }
  }
}
