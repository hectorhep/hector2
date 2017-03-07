#include "Propagator.h"

namespace Hector
{
  Propagator::Propagator( const Beamline* beamline ) :
    beamline_( beamline )
  {}

  Propagator::~Propagator()
  {}

  void
  Propagator::propagate( Particle& part, float s ) const
  {
    part.clear();

    /*const double energy_loss = ( Constants::use_relative_energy )
      ? Constants::beam_energy-part.lastStateVector().energy()
      : part.lastStateVector().energy();*/
    const double energy_loss = Parameters::beam_energy-part.lastStateVector().energy();

    for ( Beamline::ElementsMap::const_iterator it=beamline_->begin()+1; it!=beamline_->end(); it++ ) {
      // extract the previous and the current element in the beamline
      const Element::ElementBase *prev_elem = *( it-1 ), *elem = *( it );

      const Particle::Position in_pos = part.lastPosition();
      const float first_s = part.firstS();

      Particle::Position out_pos( -1., StateVector() );

      // between two elements
      if ( first_s>prev_elem->s() and first_s<elem->s() ) {
        switch ( prev_elem->type() ) {
          case Element::aDrift: { PrintInfo( Form( "Path start inside element %s", prev_elem->name().c_str() ) ); } break;
          default:              { PrintInfo( Form( "Path start inside drift %s", prev_elem->name().c_str() ) ); } break;
        }

        // build a temporary element mimicking the drift effect
        Element::ElementBase* elem_tmp = prev_elem->clone();
        elem_tmp->setS( first_s );
        elem_tmp->setLength( elem->s()-first_s );
        out_pos = propagateThrough( in_pos, elem_tmp, energy_loss, part.charge() );
        if ( elem_tmp ) delete elem_tmp;
      }
      // before one element
      if ( first_s<=elem->s() ) {
        out_pos = propagateThrough( in_pos, elem, energy_loss, part.charge() );
      }
      if ( out_pos.s()<0. ) continue; // no new point to add to the particle's trajectory

      if ( s<=out_pos.s() ) { // we are in the next element (-> interpolate to get the state vector at the requested s)
        const float l = out_pos.s()-in_pos.s();
        if ( l==0 ) {
          throw Exception( __PRETTY_FUNCTION__, Form( "No luck in choosing position (s=%.3f m)\n\tPropagation is impossible!", s ), JustWarning );
        }
        const CLHEP::Hep2Vector in = in_pos.stateVector().position(),
                                out = out_pos.stateVector().position();
        const CLHEP::Hep2Vector s_pos = in + ( ( s-in_pos.s() )/l )*( out-in );
        StateVector out_stvec( in_pos.stateVector() );
        out_stvec.setPosition( s_pos );
        return;
      }

      part.addPosition( out_pos );
    }
    part.sortPositions();
  }

  bool
  Propagator::stopped( Particle& part, float max_s ) const
  {
    bool has_stopped = false;
    for ( Beamline::ElementsMap::const_iterator it=beamline_->begin()+1; it!=beamline_->end(); it++ ) {
      // extract the previous and the current element in the beamline
      const Element::ElementBase *prev_elem = *( it-1 ), *elem = *( it );
      if ( max_s>0 and elem->s()>max_s ) return false;

      const StateVector sv_prev_elem_in = part.stateVectorAt( prev_elem->s() ),
                        sv_prev_elem_out = part.stateVectorAt( prev_elem->s()+prev_elem->length() );

      //std::cout << "---> " << sv_prev_elem_in << std::endl;
      const Aperture::ApertureBase* aper = prev_elem->aperture();
      if ( aper and aper->type()!=Aperture::anInvalidType ) {
        const bool has_passed_entrance = aper->contains( sv_prev_elem_in.position() ),
                   has_passed_exit = aper->contains( sv_prev_elem_out.position() );

        if ( !has_passed_entrance ) { throw Exception( __PRETTY_FUNCTION__, Form( "Particle stopped at the entrance of %s", prev_elem->name().c_str() ), JustWarning ); }
        if ( !has_passed_exit ) { PrintInfo( Form( "Particle stopped inside %s", prev_elem->name().c_str() ) ); }

        has_stopped = ( !has_passed_entrance or !has_passed_exit );
        /*if ( !has_passed_exit ) {
          try {
            out_pos = hitPosition( out_pos.second, elem, energy_loss );
            out_pos.first += elem_s;
          } catch ( Exception& e ) {}
        }*/
      }
    }
    return has_stopped;
  }

  Particle::Position
  Propagator::propagateThrough( const Particle::Position& ini_pos, const Element::ElementBase* ele, float eloss, int qp ) const
  {
    StateVector shift( ele->position(), math::tan2( ele->angles() ), 0. );
    //shift.setAngles( ele->angles() );

    // perform the propagation (assuming that mass is conserved...)
    StateVector vec( ele->matrix( eloss, ini_pos.stateVector().m(), qp ) * ( ini_pos.stateVector().vector()-shift.vector() ) + shift.vector(),
                     ini_pos.stateVector().m() );

    // convert the angles -> tan-1( angle )
    const CLHEP::Hep2Vector ang_old = vec.angles();
    vec.setAngles( math::atan2( ang_old ) );

    return Particle::Position( ele->s()+ele->length(), vec );
  }

  Particle::Position
  Propagator::hitPosition( const StateVector& ini_pos, const Element::ElementBase* ele, float eloss, float mp, int qp ) const
  {
    // first check the validity of the element
    if ( ele->length()<=0. ) {
      throw Exception( __PRETTY_FUNCTION__, Form( "Element %s has invalid length: %f m", ele->name().c_str(), ele->length() ), JustWarning );
    }

    StateVector vec = ini_pos;
    vec.setEnergy( -eloss );

    Element::ElementBase* ele_tmp = ele->clone();
    double min_pos = 0.,
           max_pos = ele_tmp->length()/2.,
           max_old = max_pos;
    const size_t niter = 10;

    bool inside = false;
    for ( size_t i=0; i<niter; i++ ) {

      ele_tmp->setLength( max_pos );
      StateVector shift( ele_tmp->position(), math::tan2( ele_tmp->angles() ) );

      const StateVector max = StateVector( ele_tmp->matrix( eloss, mp, qp ) * ( vec.vector()-shift.vector() ) + shift.vector(), vec.m() );

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
    StateVector shift( ele_tmp->position(), CLHEP::Hep2Vector( tan( ele_tmp->Tx() ), tan( ele_tmp->Ty() ) ) );
    
    const StateVector stop = StateVector( ele_tmp->matrix( eloss, mp, qp ) * ( vec.vector()-shift.vector() ) + shift.vector(), vec.m() );

    return Particle::Position( ( max_pos+min_pos )/2., ini_pos );
  }

  void
  Propagator::propagate( Particles& beam ) const
  {
    for ( Particles::iterator it=beam.begin(); it!=beam.end(); it++ ) {
      propagate( *it, it->firstS() );
    }
  }
}

