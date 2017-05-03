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

      const float sq_k = sqrt( ke ), inv_sq_k = 1./sq_k,
                  omega = sqrt( fabs( ke ) )*length_,
                  s_omega = sin( omega ), c_omega = cos( omega ),
                  sh_omega = sinh( omega ), ch_omega = cosh( omega );

      CLHEP::HepMatrix mat = CLHEP::HepDiagMatrix( 6, 1 );
      // Focussing Twiss matrix
      mat( 1, 1 ) = c_omega;
      mat( 1, 2 ) = s_omega * inv_sq_k;
      mat( 2, 1 ) = s_omega * ( -sq_k );
      mat( 2, 2 ) = c_omega;
      // Defocussing Twiss matrix
      mat( 3, 3 ) = ch_omega;
      mat( 3, 4 ) = sh_omega * inv_sq_k;
      mat( 4, 3 ) = sh_omega * sq_k;
      mat( 4, 4 ) = ch_omega;
      return mat;
    }

    CLHEP::HepMatrix
    VerticalQuadrupole::matrix( float eloss, float mp, int qp ) const
    {
      const float ke = fieldStrength( eloss, mp, qp );
      if ( ke==0. ) { return Drift::genericMatrix( length_ ); } // simple drift matrix

      const float sq_k = sqrt( ke ), inv_sq_k = 1./sq_k,
                  omega = sqrt( fabs( ke ) )*length_,
                  s_omega = sin( omega ), c_omega = cos( omega ),
                  sh_omega = sinh( omega ), ch_omega = cosh( omega );

      CLHEP::HepMatrix mat = CLHEP::HepDiagMatrix( 6, 1 );
      // Defocussing Twiss matrix
      mat( 1, 1 ) = ch_omega;
      mat( 1, 2 ) = sh_omega * inv_sq_k;
      mat( 2, 1 ) = sh_omega * sq_k;
      mat( 2, 2 ) = ch_omega;
      // Focussing Twiss matrix
      mat( 3, 3 ) = c_omega;
      mat( 3, 4 ) = s_omega * inv_sq_k;
      mat( 4, 3 ) = s_omega * ( -sq_k );
      mat( 4, 4 ) = c_omega;
      return mat;
    }
  }
}
