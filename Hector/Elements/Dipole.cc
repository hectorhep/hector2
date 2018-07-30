#include "Hector/Elements/Dipole.h"
#include "Hector/Elements/Drift.h"

#include "Hector/Core/Exception.h"
#include "Hector/Core/Algebra.h"

namespace Hector
{
  namespace Element
  {
    Matrix
    SectorDipole::matrix( double eloss, double mp, int qp ) const
    {
      Matrix mat = Drift::genericMatrix( length_ );

      if ( Parameters::get()->enableDipoles() == false )
        return mat;

      const double ke = fieldStrength( eloss, mp, qp );
      if ( ke == 0. ) { // simple drift matrix
        PrintDebug( Form( "Sector dipole %s has no effect. Treating it as a drift.", name_.c_str() ) );
        return mat;
      }

      const double radius = 1./ke;
      const double theta = length_*ke, s_theta = sin( theta ), c_theta = cos( theta );
      const double inv_energy = 1. / Parameters::get()->beamEnergy();

      mat( 1, 1 ) = c_theta;
      mat( 1, 2 ) = s_theta * radius;
      mat( 2, 1 ) = s_theta * ( -ke );
      mat( 2, 2 ) = c_theta;
      if ( Parameters::get()->useRelativeEnergy() ) {
        const double simp = 2.*radius*pow( sin( theta*0.5 ), 2 ) * inv_energy;
        // numerically stable version of ( r/E₀ )*( 1-cos θ )
        mat( 1, 5 ) = simp;
        mat( 2, 5 ) = s_theta * inv_energy;
      }
      return mat;
    }

    Matrix
    RectangularDipole::matrix( double eloss, double mp, int qp ) const
    {
      Matrix mat = Drift::genericMatrix( length_ );

      if ( Parameters::get()->enableDipoles() == false )
        return mat;

      const double ke = fieldStrength( eloss, mp, qp );
      if ( ke == 0. ) { // simple drift matrix
        PrintDebug( Form( "Rectangular dipole %s has no effect. Treating it as a drift.", name_.c_str() ) );
        return mat;
      }

      const double radius = 1./ke;
      const double theta = length_*ke, s_theta = sin( theta ), c_theta = cos( theta );
      //std::cout << name_ << "|" << eloss << "|" << radius << "|" << ke << "|" << theta << "|" << s_theta << "|" << c_theta << std::endl;
      const double inv_energy = 1. / Parameters::get()->beamEnergy();
      // numerically stable version of ( r/E₀ )*( 1-cos θ )
      const double simp = 2.*radius*pow( sin( theta*0.5 ), 2 ) * inv_energy;

      mat( 1, 1 ) = c_theta;
      mat( 1, 2 ) = s_theta * radius;
      mat( 2, 1 ) = s_theta * ( -ke );
      mat( 2, 2 ) = c_theta;
      mat( 1, 5 ) = simp;
      mat( 2, 5 ) = s_theta * inv_energy;

      if ( Parameters::get()->useRelativeEnergy() ) {
        /*throw Exception( __PRETTY_FUNCTION__, "Relative energy mode not yet supported in this version of Hector!\n\t"
                                              "Please contact the developers for more information.", Fatal );*/
        Matrix ef_matrix = DiagonalMatrix( 6, 1 );
        const double t_theta_half_ke = tan( theta*0.5 ) * ke;
        ef_matrix( 2, 1 ) =  t_theta_half_ke;
        ef_matrix( 4, 3 ) = -t_theta_half_ke;
        return ef_matrix * mat * ef_matrix;
      }

      return mat;
    }
  }
}

