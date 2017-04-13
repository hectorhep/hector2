#include "Propagator.h"

namespace Hector
{
  Propagator::Propagator( const Beamline* beamline ) :
    beamline_( beamline )
  {}

  Propagator::~Propagator()
  {}

  void
  Propagator::propagate( Particle& part, float s_max ) const
  {
    part.clear();
    if ( Parameters::use_relative_energy ) {
      part.firstStateVector().setEnergy( part.firstStateVector().energy()-Parameters::beam_energy );
    }
    const double energy_loss = ( true )
      ? Parameters::beam_energy-part.lastStateVector().energy()
      : part.lastStateVector().energy();
    //const double energy_loss = Parameters::beam_energy-part.lastStateVector().energy();
    const float first_s = part.firstS();

    for ( Beamline::ElementsMap::const_iterator it=beamline_->begin()+1; it!=beamline_->end(); it++ ) {
      // extract the previous and the current element in the beamline
      const Element::ElementBase *prev_elem = *( it-1 ), *elem = *( it );

      const Particle::Position in_pos = part.lastPosition();

      // initialise the outwards position
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

      std::cout << ">>> " << out_pos.s() << " / " << s_max << std::endl;
      std::cout << prev_elem << "\n" << elem << std::endl;
      /*if ( s_max<=out_pos.s() ) { // we are in the next element (-> interpolate to get the state vector at the requested s)
        const float drift_length = out_pos.s()-in_pos.s();
        if ( drift_length==0 ) {
          throw Exception( __PRETTY_FUNCTION__, Form( "No luck in choosing position (s=%.3f m)\n\tPropagation is impossible!", s ), JustWarning );
        }
        const CLHEP::Hep2Vector in = in_pos.stateVector().position(),
                                out = out_pos.stateVector().position();
        const CLHEP::Hep2Vector s_pos = in + ( ( s_max-in_pos.s() )/drift_length )*( out-in );
        StateVector out_stvec( in_pos.stateVector() );
        out_stvec.setPosition( s_pos );
        out_pos = out_stvec;
        //return;
      }*/

      part.addPosition( out_pos );
    }
    part.sortPositions();
    //part.dump();
  }

  bool
  Propagator::stopped( Particle& part, float max_s ) const
  {
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

        if ( !has_passed_entrance ) {
          PrintInfo( Form( "Particle stopped at the entrance of %s", prev_elem->name().c_str() ) );
          return true;
        }
        if ( !has_passed_exit ) {
          PrintInfo( Form( "Particle stopped inside %s", prev_elem->name().c_str() ) );
          return true;
        }
      }
    }
    return false;
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

  void
  Propagator::propagate( Particles& beam ) const
  {
    for ( Particles::iterator it=beam.begin(); it!=beam.end(); it++ ) {
      propagate( *it, it->firstS() );
    }
  }
}

