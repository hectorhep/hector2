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

  Particle::StateVector in_pos = part.firstPosition();

  for ( Beamline::ElementsMap::const_iterator it=beamline_->begin(); it!=beamline_->end(); it++ ) {
    const Element::ElementBase* elem = *it;
    mat = mat*elem->matrix();
    Particle::StateVector out_pos( mat*in_pos );

    bool has_stopped = false;
    const Element::ApertureBase* aper = elem->aperture();
    if ( aper and aper->type()!=Element::ApertureBase::None ) {
      const bool has_passed_entrance = aper->contains( in_pos.position() ),
                 has_passed_exit = aper->contains( out_pos.position() );
      has_stopped = ( !has_passed_exit );
    }
    part.addPosition( elem->s(), out_pos, has_stopped );
    if ( has_stopped ) break;
  }
}
