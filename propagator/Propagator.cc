#include "Propagator.h"

namespace Hector
{
  Propagator::Propagator( const Beamline* beamline ) :
    beamline_( beamline )
  {
  }

  Propagator::~Propagator()
  {}

  void
  Propagator::propagate( Particle& part, float s ) const
  {
    CLHEP::HepMatrix mat = CLHEP::HepDiagMatrix( 6, 1 );
    part.clear();

    const Particle::StateVector in_pos = part.firstPosition();
    const double energy_loss = Constants::beam_energy-in_pos.energy();

    for ( Beamline::ElementsMap::const_iterator it=beamline_->begin(); it!=beamline_->end(); it++ ) {
      const Element::ElementBase* elem = *it;
      mat = mat*elem->matrix( energy_loss, in_pos.momentum().m(), part.charge() );

      Particle::StateVector out_pos( mat*in_pos, in_pos.m() );
      double elem_s = elem->s();

      bool has_stopped = false;
      const Aperture::ApertureBase* aper = elem->aperture();
      if ( aper and aper->type()!=Aperture::ApertureBase::None ) {
        const bool has_passed_entrance = aper->contains( in_pos.position() ),
                   has_passed_exit = aper->contains( out_pos.position() );

        if ( !has_passed_entrance or !has_passed_exit ) has_stopped = true;
        if ( !has_passed_exit ) {
          std::pair<float,Particle::StateVector> hp = hitPosition( out_pos, elem, energy_loss, in_pos.momentum().m(), part.charge() );
          elem_s += hp.first;
          out_pos = hp.second;
        }

      }
      try {
        part.addPosition( elem_s, out_pos, has_stopped );
      } catch ( Exception& e ) { e.dump(); }

      if ( has_stopped ) break;
    }
  }

  std::pair<float,Particle::StateVector>
  Propagator::hitPosition( const Particle::StateVector& ini_pos, const Element::ElementBase* ele, float eloss, float mp, int qp ) const
  {
    // first check the validity of the element
    if ( ele->length()<=0. ) {
      throw Exception( __PRETTY_FUNCTION__, Form( "Element %s has invalid length: %f m", ele->name(), ele->length() ), Fatal );
    }

    Particle::StateVector vec = ini_pos;
    vec.setEnergy( -eloss );

    Element::ElementBase* ele_tmp = ele->clone();
    double min_pos = 0.,
           max_pos = ele_tmp->length()/2.,
           max_old = max_pos;
    const size_t niter = 10;

    bool inside = false;
    for ( size_t i=0; i<niter; i++ ) {
      ele_tmp->setLength( max_pos );
      Particle::StateVector shift;
      shift[Particle::StateVector::X] = ele_tmp->x();
      shift[Particle::StateVector::Y] = ele_tmp->y();
      shift[Particle::StateVector::TX] = tan( ele_tmp->Tx() );
      shift[Particle::StateVector::TY] = tan( ele_tmp->Ty() );

      const Particle::StateVector max = Particle::StateVector( ele_tmp->matrix( eloss, mp, qp ) * ( vec-shift ) + shift, vec.m() );

      if ( ele_tmp->aperture() ) {
        inside = ele_tmp->aperture()->contains( max.position() );
        if ( inside ) {
          max_old = max_pos;
          max_pos += ( max_pos-min_pos )/2.;
          min_pos = max_old;
        }
        else {
          max_pos = min_pos + ( max_pos-min_pos )/2.;
        }
      }
    }

    if ( inside ) min_pos = max_old;

    // estimate state vector in the middle
    ele_tmp->setLength( ( max_pos-min_pos )/2. );
    Particle::StateVector shift;
    shift[Particle::StateVector::X] = ele_tmp->x();
    shift[Particle::StateVector::Y] = ele_tmp->y();
    shift[Particle::StateVector::TX] = tan( ele_tmp->Tx() );
    shift[Particle::StateVector::TY] = tan( ele_tmp->Ty() );
    
    const Particle::StateVector stop = Particle::StateVector( ele_tmp->matrix( eloss, mp, qp ) * ( vec-shift ) + shift, vec.m() );

    return std::pair<float,Particle::StateVector>( ( max_pos+min_pos )/2., ini_pos );
  }
}

