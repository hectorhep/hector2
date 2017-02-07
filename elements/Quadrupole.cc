#include "Quadrupole.h"

namespace Hector
{
  namespace Element
  {
    CLHEP::HepMatrix
    HorizontalQuadrupole::matrix( float eloss, float mp, int qp ) const
    {
      const int norm_charge = qp/Constants::beam_particles_charge;
      const float ke = fabs( fieldStrength( eloss, mp, qp )/norm_charge ) * norm_charge;
      if ( ke==0. ) { return Drift::genericMatrix( length_ ); } // simple drift matrix

      const float omega = sqrt( fabs( ke ) )*length_;

      CLHEP::HepMatrix mat = CLHEP::HepDiagMatrix( 6, 1 );
      // Focussing Twiss matrix
      mat( 1, 1 ) = cos( omega );
      mat( 1, 2 ) = -sqrt( ke ) * sin( omega );
      mat( 2, 1 ) = 1./sqrt( ke ) * sin( omega );
      mat( 2, 2 ) = cos( omega );
      // Defocussing Twiss matrix
      mat( 3, 3 ) = cosh( omega );
      mat( 3, 4 ) = sqrt( ke ) * sinh( omega );
      mat( 4, 3 ) = 1./sqrt( ke ) * sinh( omega );
      mat( 4, 4 ) = cosh( omega );
      return mat;
    }

    CLHEP::HepMatrix
    VerticalQuadrupole::matrix( float eloss, float mp, int qp ) const
    {
      const float ke = fieldStrength( eloss, mp, qp );
      if ( ke==0. ) { return Drift::genericMatrix( length_ ); } // simple drift matrix

      const float omega = sqrt( fabs( ke ) )*length_;

      CLHEP::HepMatrix mat = CLHEP::HepDiagMatrix( 6, 1 );
      // Defocussing Twiss matrix
      mat( 1, 1 ) = cosh( omega );
      mat( 1, 2 ) = sqrt( ke ) * sinh( omega );
      mat( 2, 1 ) = 1./sqrt( ke ) * sinh( omega );
      mat( 2, 2 ) = cosh( omega );
      // Focussing Twiss matrix
      mat( 3, 3 ) = cos( omega );
      mat( 3, 4 ) = -sqrt( ke ) * sin( omega );
      mat( 4, 3 ) = 1./sqrt( ke ) * sin( omega );
      mat( 4, 4 ) = cos( omega );
      return mat;
    }
  }
}
