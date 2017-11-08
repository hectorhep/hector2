#include "Hector/Elements/Dipole.h"
#include "Hector/Elements/Drift.h"
#include "Hector/Core/Exception.h"

#include <CLHEP/Matrix/DiagMatrix.h>

namespace Hector
{
  namespace Element
  {
    CLHEP::HepMatrix
    SectorDipole::matrix( float eloss, float mp, int qp ) const
    {
      if ( !Parameters::get()->enableDipoles() ) return Drift::genericMatrix( length_ );

      const float ke = fieldStrength( eloss, mp, qp ); //FIXME

      if ( ke==0. ) { // simple drift matrix
        Exception( __PRETTY_FUNCTION__, Form( "Dipole %s has no effect. Treating it as a drift.", name_.c_str() ), JustWarning ).dump();
        return Drift::genericMatrix( length_ );
      }

      const double radius = 1./ke,
                   theta = length_*ke,
                   c_theta = cos( theta ), s_theta = sin( theta ),
        inv_energy = 1. / Parameters::get()->beamEnergy();

      CLHEP::HepMatrix mat = CLHEP::HepDiagMatrix( 6, 1 );

      mat( 1, 1 ) = c_theta;
      mat( 1, 2 ) = s_theta * radius;
      mat( 2, 1 ) = s_theta * ( -ke );
      mat( 2, 2 ) = c_theta;
      mat( 3, 4 ) = length_;
      if ( Parameters::get()->useRelativeEnergy() ) {
        const double simp = 2.*radius*pow( sin( theta*0.5 ), 2 ) * inv_energy;
        // numerically stable version of ( r/E₀ )*( 1-cos θ )
        mat( 1, 5 ) = simp;
        mat( 2, 5 ) = s_theta * inv_energy;
      }
      return mat;
    }

    CLHEP::HepMatrix
    RectangularDipole::matrix( float eloss, float mp, int qp ) const
    {
      if ( !Parameters::get()->enableDipoles() ) return Drift::genericMatrix( length_ );

      const float ke = fieldStrength( eloss, mp, qp ); //FIXME

      if ( ke==0. ) { // simple drift matrix
        Exception( __PRETTY_FUNCTION__, Form( "Dipole %s has no effect. Treating it as a drift.", name_.c_str() ), JustWarning ).dump();
        return Drift::genericMatrix( length_ );
      }

      const double radius = 1./ke,
                   theta = length_*ke,
                   s_theta = sin( theta ), c_theta = cos( theta ),
                   inv_energy = 1. / Parameters::get()->beamEnergy(),
                   simp = 2.*radius*pow( sin( theta*0.5 ), 2 ) * inv_energy;
                   // numerically stable version of ( r/E₀ )*( 1-cos θ )

      CLHEP::HepMatrix mat = CLHEP::HepDiagMatrix( 6, 1 );

      mat( 1, 1 ) = c_theta;
      mat( 1, 2 ) = s_theta * radius;
      mat( 2, 1 ) = s_theta * ( -ke );
      mat( 2, 2 ) = c_theta;
      mat( 3, 4 ) = length_;
      mat( 1, 5 ) = simp;
      mat( 2, 5 ) = s_theta * inv_energy;

      if ( Parameters::get()->useRelativeEnergy() ) {
        /*throw Exception( __PRETTY_FUNCTION__, "Relative energy mode not yet supported in this version of Hector!\n\t"
                                                "Please contact the developers for more information.", Fatal );*/
        CLHEP::HepMatrix ef_matrix = CLHEP::HepDiagMatrix( 6, 1 );
        const double t_theta_half_ke = tan( theta*0.5 ) * ke;
        ef_matrix( 2, 1 ) =  t_theta_half_ke;
        ef_matrix( 4, 3 ) = -t_theta_half_ke;

        return ef_matrix * mat * ef_matrix;
      }

      return mat;
    }
  }
}
