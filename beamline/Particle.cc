#include "Particle.h"

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
