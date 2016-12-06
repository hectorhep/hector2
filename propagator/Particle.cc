#include "Particle.h"

namespace Hector
{
  Particle::Particle() :
    charge_( 0 ), stopped_( false )
  {
  }

  Particle::Particle( const StateVector& sv0 ) :
    pos0_( sv0 ), charge_( 0 ), stopped_( false )
  {
    addPosition( 0., pos0_ );
  }


  Particle::~Particle()
  {}

  void
  Particle::addPosition( float s, const StateVector& sv, bool stopped )
  {
    if ( positions_.size()>0 and lastPosition().m()!=sv.m() ) {
      throw Exception( __PRETTY_FUNCTION__, Form( "Particle mass is not conserved in propagation!\n\tPrevious mass was %.3f GeV, new mass is %.3f GeV", lastPosition().m(), sv.m() ), Fatal );
    }
    positions_.insert( std::pair<float, StateVector>( s, sv ) );
    stopped_ = stopped;
  }

  void
  Particle::dump( std::ostream& os ) const
  {
    os << "Particle of charge " << charge_ << "\n"
       << " initial position: " << pos0_.T() << "\n"
       << " list of associated state vectors:\n";
    for ( std::map<float, StateVector>::const_iterator it=positions_.begin(); it!=positions_.end(); it++ ) {
      std::cout << "   s = " << it->first << " m: " << it->second.T() << "\n";
    }
  }
}
