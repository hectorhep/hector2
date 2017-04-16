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
    /*if ( Parameters::use_relative_energy ) {
      part.firstStateVector().setEnergy( part.firstStateVector().energy()-Parameters::beam_energy );
    }*/
    const bool non_linear = true;
    const double energy_loss = ( non_linear )
      ? Parameters::beam_energy-part.lastStateVector().energy()
      : 0.;
    //const double energy_loss = Parameters::beam_energy-part.lastStateVector().energy();
    //part.firstStateVector().setEnergy( energy_loss );
    const float first_s = part.firstS();

    try {

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

        part.addPosition( out_pos );

        const Aperture::ApertureBase* aper = prev_elem->aperture();
        if ( aper and aper->type()!=Aperture::anInvalidType ) {
          const CLHEP::Hep2Vector pos_prev_elem( part.stateVectorAt( prev_elem->s() ).position() );
          if ( !aper->contains( pos_prev_elem ) ) {
            std::ostringstream os; os << pos_prev_elem;
            throw Exception( __PRETTY_FUNCTION__, Form( "Particle stopped at the entrance of %s.\n\tEntering at %s", prev_elem->name().c_str(), os.str().c_str() ), JustWarning );
          }
          // has passed through the element?
          if ( !aper->contains( part.stateVectorAt( prev_elem->s()+prev_elem->length() ).position() ) ) {
            throw Exception( __PRETTY_FUNCTION__, Form( "Particle stopped inside %s", prev_elem->name().c_str() ), JustWarning );
          }
        }
      }
    } catch ( Exception& e ) { e.dump(); }

    // finish by sorting all positions according to their s-coordinate
    part.sortPositions();
  }

  bool
  Propagator::stopped( Particle& part, float s_max ) const
  {
    for ( Beamline::ElementsMap::const_iterator it=beamline_->begin()+1; it!=beamline_->end(); it++ ) {
      // extract the previous and the current element in the beamline
      const Element::ElementBase *prev_elem = *( it-1 ), *elem = *( it );
      if ( s_max>0 and elem->s()>s_max ) return false;

      const Aperture::ApertureBase* aper = prev_elem->aperture();
      if ( aper and aper->type()!=Aperture::anInvalidType ) {

        // has passed the element entrance?
        if ( !aper->contains( part.stateVectorAt( prev_elem->s() ).position() ) ) {
          //PrintInfo( Form( "Particle stopped at the entrance of %s", prev_elem->name().c_str() ) );
          return true;
        }
        // has passed through the element?
        if ( !aper->contains( part.stateVectorAt( prev_elem->s()+prev_elem->length() ).position() ) ) {
          //PrintInfo( Form( "Particle stopped inside %s", prev_elem->name().c_str() ) );
          return true;
        }
      }
    }
    return false;
  }

  Particle::Position
  Propagator::propagateThrough( const Particle::Position& ini_pos, const Element::ElementBase* elem, float eloss, int qp ) const
  {
    const StateVector shift( elem->position(), math::tan2( elem->angles() ), 0., 0. );

    /*std::ostringstream os1, os2;
    os1 << elem->type(); os2 << elem->matrix( eloss, ini_pos.stateVector().m(), qp );
    PrintInfo( Form( "Propagating through %s element \"%s\" with transfer matrix\n%s", os1.str().c_str(), elem->name().c_str(), os2.str().c_str() ) );*/

//std::cout << "matrix for " << elem->type() << ": " << elem->matrix( eloss, ini_pos.stateVector().m(), qp ) << ", " << elem->matrix( eloss, ini_pos.stateVector().m(), qp ).T()*elem->matrix( eloss, ini_pos.stateVector().m(), qp ) << " ; " << elem->matrix( eloss, ini_pos.stateVector().m(), qp ) * elem->matrix( eloss, ini_pos.stateVector().m(), qp ).T() << std::endl;

    CLHEP::HepVector prop = elem->matrix( eloss, ini_pos.stateVector().m(), qp ) * ( ini_pos.stateVector().vector()-shift.vector() ) + shift.vector();
    // perform the propagation (assuming that mass is conserved...)
    StateVector vec( prop, ini_pos.stateVector().m() );

    // convert the angles -> tan-1( angle )
    const CLHEP::Hep2Vector ang_old = vec.angles();
    vec.setAngles( math::atan2( ang_old ) );

    return Particle::Position( elem->s()+elem->length(), vec );
  }

  void
  Propagator::propagate( Particles& beam, float s_max ) const
  {
    for ( Particles::iterator it=beam.begin(); it!=beam.end(); it++ ) {
      propagate( *it, s_max );
    }
  }
}

