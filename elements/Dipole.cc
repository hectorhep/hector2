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
      mat( 1, 2 ) = sin( theta ) * radius;
      mat( 2, 1 ) = sin( theta ) * ( -1./radius );
      mat( 2, 2 ) = cos( theta );
      mat( 3, 4 ) = length_;
      if ( Parameters::use_relative_energy ) {
        const float simp = 2.*radius*pow( sin( theta/2. ), 2 ) / Parameters::beam_energy;
        // numerically stable version of ( r/E₀ )*( 1-cos θ )
        mat( 1, 5 ) = simp;
        mat( 2, 5 ) = sin( theta ) / Parameters::beam_energy;
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
                  simp = 2.*radius*pow( sin( theta/2. ), 2 ) / Parameters::beam_energy;

      CLHEP::HepMatrix mat = CLHEP::HepDiagMatrix( 6, 1 );

      mat( 1, 1 ) = cos( theta );
      mat( 1, 2 ) = sin( theta ) * radius;
      mat( 2, 1 ) = sin( theta ) * ( -1./radius );
      mat( 2, 2 ) = cos( theta );
      mat( 3, 4 ) = length_;
      mat( 1, 5 ) = simp;
      mat( 2, 5 ) = sin( theta ) / Parameters::beam_energy;

      if ( Parameters::use_relative_energy ) {
        throw Exception( __PRETTY_FUNCTION__, "Relative energy mode not yet supported in this version of Hector!\n\t"
                                              "Please contact the developers for more information.", Fatal );
        const float psy = ke*length_/2.;

        CLHEP::HepMatrix ef_matrix = CLHEP::HepDiagMatrix( 6, 1 );
        ef_matrix( 2, 1 ) =  tan( psy ) * ke;
        ef_matrix( 4, 3 ) = -tan( psy ) * ke;

        return ef_matrix * mat * ef_matrix;
      }

      return mat;
    }
  }
}
