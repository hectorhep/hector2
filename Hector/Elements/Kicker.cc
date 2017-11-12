#include "Hector/Elements/Kicker.h"
#include "Hector/Elements/Drift.h"
#include "Hector/Core/Parameters.h"

namespace Hector
{
  namespace Element
  {
    Matrix
    HorizontalKicker::matrix( float eloss, float mp, int qp ) const
    {
      if ( !Parameters::get()->enableKickers() ) return Drift::genericMatrix( length_ );

      const float ke = -fieldStrength( eloss, mp, qp );
      if ( ke==0 ) return Drift::genericMatrix( length_ );

      Matrix mat = Drift::genericMatrix( length_ );

      mat( 1, 6 ) = length_*tan( ke ) * 0.5;
      mat( 2, 6 ) = ke;
      return mat;
    }

    Matrix
    VerticalKicker::matrix( float eloss, float mp, int qp ) const
    {
      if ( !Parameters::get()->enableKickers() ) return Drift::genericMatrix( length_ );

      const float ke = -fieldStrength( eloss, mp, qp );
      if ( ke==0 ) return Drift::genericMatrix( length_ );

      Matrix mat = Drift::genericMatrix( length_ );

      mat( 3, 6 ) = length_*tan( ke ) * 0.5;
      mat( 4, 6 ) = ke;
      return mat;
    }
  }
}
