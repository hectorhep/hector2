#include "Hector/Core/Utils.h"
#include "Hector/Core/Exception.h"
#include "Hector/Propagator/Particle.h"

#include <stdarg.h>  // For va_start, etc.
#include <algorithm>
#include <locale>
#include <CLHEP/Random/RandFlat.h>

namespace Hector
{
  std::string Form( const std::string fmt, ... )
  {
    int size = ( static_cast<int>( fmt.size() ) )*2 + 50;
    std::string str;
    va_list ap;
    while ( true ) {
      str.resize( size );
      va_start( ap, fmt );
      int n = vsnprintf( (char*)str.data(), size, fmt.c_str(), ap );
      va_end( ap );
      if ( n>-1 and n<size ) {
        str.resize( n );
        return str;
      }
      if ( n>-1 ) size = n + 1;
      else size *= 2; // retry with a larger size
    }
    return str;
  }

  std::string trim( const std::string& str ) {
    if ( str=="" ) return str;
    size_t first = str.find_first_not_of( ' ' ),
           last = str.find_last_not_of( ' ' );
    std::string tmp = str.substr( first, ( last-first+1 ) );
    first = tmp.find_first_not_of( '"' );
    last = tmp.find_last_not_of( '"' );
    return tmp.substr( first, ( last-first+1 ) );
  }

  /// Get the lowercase version of a string
  std::string lowercase( const std::string& str ) {
    std::string s( str );
    std::transform( str.begin(), str.end(), s.begin(), ::tolower );
    return s;
  }

  void
  emitGamma( Particle& part, double e_gamma, double q2_gamma, double phi_min, double phi_max )
  {
    const double pos_ini = part.firstS();
    StateVector sv_ini = part.firstStateVector();

    if ( q2_gamma == 0. ) {
      PrintInfo( "Virtuality is null: only energy has changed" );
      sv_ini.setEnergy( sv_ini.energy()-e_gamma );
      return;
    }
    const double m1 = Parameters::get()->beamParticlesMass(),
                 e1 = sv_ini.energy(),
                 e2 = sv_ini.energy()-e_gamma; // particle energy : before (1) / after (2)

    const double p1 = sqrt( e1*e1-m1*m1 ),
                 p2 = sqrt( e2*e2-m1*m1 ); // particle momentum : before (1) / after (2)
    const double q2min = e_gamma*e_gamma-pow( p1+p2 , 2 ), // lower bound from kinematics E^2 - (p1 + p2)^2
                 q2max = -2*pow( m1*e_gamma/( p1+p2 ), 2 ) * ( 1.+( e1*e1+e2*e2-m1*m1 )/( e1*e2+p1*p2 ) );
                 // upper bound from kinematics ; E^2 - (p1-p2)^2; is bad for numerical computations

    // if q2min < q2 < q2max is NOT true, there will be mathematical problems (like cos(eta) > 1).
    // so if q2 > q2max, we force q2 = q2max (-> cos(eta) = 1)
    // and if q2 < q2min, we force q2 = q2min (-> cos(eta) = 1)
    // BUT the user knows something was wrong with the value of "H_BeamParticle::isphysical"

    /*bool is_physical = true;
    if ( ( q2_gamma>q2max ) or ( q2_gamma<q2min ) ) {
      PrintInfo( Form( "WARNING: Non physical particle! Q2=%.5f GeV**2 and E=%.5f GeV are not compatible.", q2_gamma, e_gamma ) );
      is_physical = false;
    }*///FIXME

    const double q2 = ( q2_gamma > q2max ) ? q2max : ( q2_gamma<q2min ) ? q2min : q2_gamma;

    sv_ini.setEnergy( e2 );
    // gkk is k
    const double gkk = sqrt( e_gamma*e_gamma-q2 );
    // eta is the angle between gamma and initial direction of the gamma-emitting particle
    // ceta = cos(eta) and seta = sin(eta)

    const double ceta = sqrt( 1.+pow( m1/p1, 2 ) )*sqrt( 1.+q2/gkk*gkk ) - q2/( 2*p1*gkk ),
                 seta = sqrt( 1.-ceta*ceta );
    // theta is the angle between particle and beam
    const double theta = atan( seta/( Parameters::get()->beamEnergy()/gkk-ceta ) ),
                 phi = CLHEP::RandFlat::shoot( phi_min, phi_max );

    TwoVector old_ang( sv_ini.angles() );
    sv_ini.setAngles( old_ang + TwoVector( theta*cos( phi ), -theta*sin( phi ) ) );

    // caution: emitting a photon erases all known positions !
    part.clear();
    part.addPosition( pos_ini, sv_ini );
  }
}
