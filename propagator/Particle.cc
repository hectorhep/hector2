#include "Particle.h"

namespace Hector
{
  Particle::Particle() :
    charge_( 0 ), pdgId_( 0 ), stopped_( false )
  {}

  Particle::Particle( const StateVector& sv0, float s0 ) :
    charge_( 0 ), pdgId_( 0 ), stopped_( false )
  {
    addPosition( s0, sv0 );
  }


  Particle::~Particle()
  {}

  Particle
  Particle::fromMassCharge( float mass, float charge )
  {
    Particle p( StateVector( CLHEP::HepVector( 6, 0 ), mass ) );
    p.setCharge( charge );
    return p;
  }

  void
  Particle::addPosition( const Position& pos, bool stopped )
  {
    if ( positions_.size()>0 and lastStateVector().m()!=pos.stateVector().m() ) {
      throw Exception( __PRETTY_FUNCTION__, Form( "Particle mass is not conserved in propagation!\n\t"
                                                  "Previous mass was %.3f GeV, new mass is %.3f GeV",
                                                  lastStateVector().m(), pos.stateVector().m() ), Fatal );
    }
    positions_.insert( pos.pair() );
    stopped_ = stopped;
  }

  StateVector
  Particle::stateVectorAt( float s ) const
  {
    PositionsMap::const_iterator it = positions_.find( s );
    if ( it!=positions_.end() ) return it->second;

    PositionsMap::const_iterator lower_it = --positions_.upper_bound( s ),
                                 upper_it = positions_.upper_bound( s );

    if ( lower_it==positions_.end() ) return StateVector();

    //PrintInfo( Form( "Interpolating for s = %.2f between %.2f and %.2f", s, lower_it->first, upper_it->first ) );

    const StateVector sv_before = lower_it->second,
                      sv_after = upper_it->second;
    const CLHEP::Hep2Vector in = sv_before.position(),
                            out = sv_after.position();

    const float drift_length = upper_it->first-lower_it->first;
    if ( drift_length==0 ) {
      throw Exception( __PRETTY_FUNCTION__, Form( "No luck in choosing position (s=%.3f m)\n\t"
                                                  "Interpolation is impossible!", s ), JustWarning );
    }

    const CLHEP::Hep2Vector s_pos = in + ( ( s-lower_it->first )/drift_length )*( out-in );

    StateVector out_stvec( lower_it->second );
    out_stvec.setPosition( s_pos );
    return out_stvec;
  }

  void
  Particle::dump( std::ostream& os ) const
  {
    os << "Particle of charge " << charge_ << "\n"
       << " initial position: " << firstStateVector() << "\n";
    if ( positions_.size()==1 ) return;
    os << " list of associated state vectors:\n";
    for ( PositionsMap::const_iterator it=positions_.begin(); it!=positions_.end(); it++ ) {
      os << Form( "   s = %8.3f m:", it->first ) << " " << it->second << std::endl;
    }
  }

  void
  Particle::emitGamma( float e_gamma, float q2_gamma, float phi_min, float phi_max )
  {
    const float pos_ini = firstS();
    StateVector sv_ini = firstStateVector();

    if ( q2_gamma==0. ) {
      PrintInfo( "Virtuality is null: only energy has changed" );
      sv_ini.setEnergy( sv_ini.energy()-e_gamma );
      return;
    }
    const double m1 = Parameters::beam_particles_mass,
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

    const double q2 = ( q2_gamma>q2max ) ? q2max : ( q2_gamma<q2min ) ? q2min : q2_gamma;

    bool has_emitted = false;
    if ( has_emitted ) {
      PrintInfo( "WARNING: particle has already emitted at least one gamma!" );
    }
    has_emitted = true;
    sv_ini.setEnergy( e2 );
    // gkk is k
    const double gkk = sqrt( e_gamma*e_gamma-q2 );
    // eta is the angle between gamma and initial direction of the gamma-emitting particle
    // ceta = cos(eta) and seta = sin(eta)

    const double ceta = sqrt( 1.+pow( m1/p1, 2 ) )*sqrt( 1.+q2/gkk*gkk ) - q2/( 2*p1*gkk ),
                 seta = sqrt( 1.-ceta*ceta );
    // theta is the angle between particle and beam
    const double theta = atan( seta/( Parameters::beam_energy/gkk-ceta ) ),
                 phi = CLHEP::RandFlat::shoot( phi_min, phi_max );

    CLHEP::Hep2Vector old_ang( sv_ini.angles() );
    sv_ini.setAngles( old_ang + CLHEP::Hep2Vector( theta*cos( phi ), -theta*sin( phi ) ) );

    // caution: emitting a photon erases all known positions !
    positions_.clear();
    addPosition( pos_ini, sv_ini );
  }
}
