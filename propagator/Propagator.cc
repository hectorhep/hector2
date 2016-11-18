#include "Propagator.h"

Propagator::Propagator( const Beamline* beamline ) :
  beamline_( beamline )
{
}

Propagator::~Propagator()
{}

void
Propagator::propagate( Particle& part, float s )
{
  CLHEP::HepMatrix mat = CLHEP::HepDiagMatrix( 6, 1 );

  Particle::StateVector pos = part.firstPosition();

  for ( Beamline::ElementsMap::const_iterator it=beamline_->begin(); it!=beamline_->end(); it++ ) {
    const Element::ElementBase* elem = *it;
    mat = mat*elem->matrix();
    part.addPosition( elem->s(), Particle::StateVector( mat*pos ) );
  }
}
