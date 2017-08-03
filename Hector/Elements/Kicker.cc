#include "Kicker.h"

namespace Hector
{
  namespace Element
  {
    CLHEP::HepMatrix
    HorizontalKicker::matrix( float eloss, float mp, int qp ) const
    {
      if ( !Parameters::get()->enableKickers() ) return Drift::genericMatrix( length_ );

      const float ke = -fieldStrength( eloss, mp, qp );
      if ( ke==0 ) return Drift::genericMatrix( length_ );

      CLHEP::HepMatrix mat = Drift::genericMatrix( length_ );

      mat( 1, 6 ) = length_*tan( ke ) * 0.5;
      mat( 2, 6 ) = ke;
      return mat;
    }

    CLHEP::HepMatrix
    VerticalKicker::matrix( float eloss, float mp, int qp ) const
    {
      if ( !Parameters::get()->enableKickers() ) return Drift::genericMatrix( length_ );

      const float ke = -fieldStrength( eloss, mp, qp );
      if ( ke==0 ) return Drift::genericMatrix( length_ );

      CLHEP::HepMatrix mat = Drift::genericMatrix( length_ );

      mat( 3, 6 ) = length_*tan( ke ) * 0.5;
      mat( 4, 6 ) = ke;
      return mat;
    }
  }
}
