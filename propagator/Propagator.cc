#include "Propagator.h"

namespace Hector
{
  Propagator::Propagator( const Beamline* beamline ) :
    beamline_( beamline )
  {
  }

  Propagator::~Propagator()
  {}

  Particle::Position
  Propagator::passThrough( const Particle::Position& ini_pos, const Element::ElementBase* ele, float eloss, int qp ) const
  {
    Particle::StateVector shift;
    shift.setPosition( ele->position() );
    shift.setAngles(
      CLHEP::Hep2Vector( tan( ele->angles().x() ), tan( ele->angles().y() ) )
    );
    CLHEP::HepVector vec( ele->matrix( eloss, ini_pos.stateVector().m(), qp ) * ( ini_pos.stateVector()-shift ) + shift );
    vec[Particle::StateVector::TX] = atan( vec[Particle::StateVector::TX] );
    vec[Particle::StateVector::TY] = atan( vec[Particle::StateVector::TY] );
    return Particle::Position( ele->s()+ele->length(), Particle::StateVector( vec, ini_pos.stateVector().m() ) ); // assume that mass is conserved here
  }


  void
  Propagator::propagate( Particle& part, float s ) const
  {
    part.clear();

    const double energy_loss = Constants::beam_energy-part.firstStateVector().energy();

    for ( Beamline::ElementsMap::const_iterator it=beamline_->begin()+1; it!=beamline_->end(); it++ ) {
      // extract the previous and the current element in the beamline
      const Element::ElementBase *prev_elem = *( it-1 ), *elem = *( it );

      const Particle::Position in_pos = part.lastPosition();
      const float first_s = part.firstS();

      Particle::Position out_pos( -1., Particle::StateVector() );

      // between two elements
      if ( in_pos.s()>prev_elem->s() and in_pos.s()<elem->s() ) {
        Element::ElementBase* elem_tmp = prev_elem->clone();
        elem_tmp->setS( first_s );
        elem_tmp->setLength( elem->s()-first_s );
        out_pos = passThrough( in_pos, elem_tmp, energy_loss, part.charge() );
        if ( elem_tmp ) delete elem_tmp;
      }
      // before one element
      if ( in_pos.s()<=elem->s() ) {
        out_pos = passThrough( in_pos, elem, energy_loss, part.charge() );
      }
      if ( out_pos.s()<0. ) continue; // no new point to add to the particle's trajectory

      // check if particle was stopped
      const Aperture::ApertureBase* aper = prev_elem->aperture();
      if ( aper and aper->type()!=Aperture::ApertureBase::None ) {
        //std::cout << in_pos.second.position() << out_pos.second.position() << std::endl;
        const bool has_passed_entrance = aper->contains( in_pos.stateVector().position() ),
                   has_passed_exit = aper->contains( out_pos.stateVector().position() );
        if ( !has_passed_entrance ) { std::cout << Form( "Particle stopped at the entrance of %s", prev_elem->name().c_str() ) << std::endl; return; }
        if ( !has_passed_exit ) { std::cout << Form( "Particle stopped inside %s", prev_elem->name().c_str() ) << std::endl; }
      }

      if ( out_pos.s()>=s ) { // we are in the next element
        const float l = out_pos.s()-in_pos.s();
        if ( l==0 ) { std::cout << Form( "No luck in choosing position (s=%.3f m) ; no propagation possible!", s ) << std::endl; return; }
        const CLHEP::Hep2Vector s_pos = in_pos.stateVector().position() + ( s-in_pos.s() )*( out_pos.stateVector().position()-in_pos.stateVector().position() )/l;
        Particle::StateVector out_s( in_pos.stateVector(), in_pos.stateVector().m() );
        out_s.setPosition( s_pos );
        part.addPosition( Particle::Position( s, out_s ) );
        std::cout << out_s.position() << std::endl;
      }
      else part.addPosition( out_pos );

      /*std::cout << ">>> " << l << "\t" << s_pos << std::endl;*/

      /*mat = mat*elem->matrix( energy_loss, part.mass(), part.charge() );

      Particle::Position out_pos( elem->s(), Particle::StateVector out_pos( mat*in_pos, in_pos.m() ) );

      bool has_stopped = false;
      const Aperture::ApertureBase* aper = elem->aperture();
      if ( aper and aper->type()!=Aperture::ApertureBase::None ) {
        const bool has_passed_entrance = aper->contains( in_pos.position() ),
                   has_passed_exit = aper->contains( out_pos.second.position() );

        has_stopped = ( !has_passed_entrance or !has_passed_exit );
        if ( !has_passed_exit ) {
          try {
            out_pos = hitPosition( out_pos.second, elem, energy_loss );
            out_pos.first += elem_s;
          } catch ( Exception& e ) {}
        }

      }
      try {
        part.addPosition( out_pos, has_stopped );
      } catch ( Exception& e ) { e.dump(); }

      if ( has_stopped ) break;*/
    }
  }

  Particle::Position
  Propagator::hitPosition( const Particle::StateVector& ini_pos, const Element::ElementBase* ele, float eloss, float mp, int qp ) const
  {
    // first check the validity of the element
    if ( ele->length()<=0. ) {
      throw Exception( __PRETTY_FUNCTION__, Form( "Element %s has invalid length: %f m", ele->name().c_str(), ele->length() ), JustWarning );
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
      shift.setPosition( ele_tmp->position() );
      shift.setAngles(
        CLHEP::Hep2Vector( tan( ele_tmp->Tx() ), tan( ele_tmp->Ty() ) )
      );

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
    shift.setPosition( ele_tmp->position() );
    shift.setAngles(
      CLHEP::Hep2Vector( tan( ele_tmp->Tx() ), tan( ele_tmp->Ty() ) )
    );
    
    const Particle::StateVector stop = Particle::StateVector( ele_tmp->matrix( eloss, mp, qp ) * ( vec-shift ) + shift, vec.m() );

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

