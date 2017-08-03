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
    /*if ( Parameters::get()->useRelativeEnergy() ) {
      part.firstStateVector().setEnergy( part.firstStateVector().energy()-Parameters::get()->beamEnergy() );
    }*/
    const bool non_linear = true;
    const double energy_loss = ( non_linear )
      ? Parameters::get()->beamEnergy()-part.lastStateVector().energy()
      : 0.;
    //part.firstStateVector().setEnergy( energy_loss );
    const float first_s = part.firstS();

    try {

      for ( Elements::const_iterator it=beamline_->begin()+1; it!=beamline_->end(); it++ ) {
        // extract the previous and the current element in the beamline
        const Element::ElementBase *prev_elem = *( it-1 ), *elem = *( it );
        if ( elem->s()>s_max ) break;

        const Particle::Position in_pos( *part.rbegin() );

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

        part.addPosition( out_pos.s(), out_pos.stateVector() );

        if ( Parameters::get()->computeApertureAcceptance() ) {
          const Aperture::ApertureBase* aper = prev_elem->aperture();
          if ( aper and aper->type()!=Aperture::anInvalidAperture ) {
            const CLHEP::Hep2Vector pos_prev_elem( part.stateVectorAt( prev_elem->s() ).position() );
            if ( !aper->contains( pos_prev_elem ) ) {
              std::ostringstream os1, os2, os3;
              os1 << prev_elem->type();
              os2 << pos_prev_elem;
              os3 << aper->position();
              throw Exception( __PRETTY_FUNCTION__,
                               Form( "Particle stopped at the entrance of %s %s.\n\t"
                                     "Entering at %s, s = %.2f m\n\t"
                                     "Aperture centre at %s\n\t"
                                     "Distance to aperture centre: %.2f cm",
                                     prev_elem->name().c_str(), os1.str().c_str(), os2.str().c_str(), prev_elem->s(), os3.str().c_str(), ( aper->position()-pos_prev_elem ).mag()*1.e2 ),
                               Info );
            }
            // has passed through the element?
            //std::cout << prev_elem->s()+prev_elem->length() << "\t" << part.stateVectorAt( prev_elem->s()+prev_elem->length() ).position() << std::endl;
            if ( !aper->contains( part.stateVectorAt( prev_elem->s()+prev_elem->length() ).position() ) ) {
              throw Exception( __PRETTY_FUNCTION__,
                               Form( "Particle stopped inside %s", prev_elem->name().c_str() ),
                               JustWarning );
            }
          }
        }
      }
    } catch ( Exception& e ) { e.dump(); }

    // finish by sorting all positions according to their s-coordinate
    //part.sortPositions();
  }

  bool
  Propagator::stopped( Particle& part, float s_max ) const
  {
    for ( Elements::const_iterator it=beamline_->begin()+1; it!=beamline_->end(); it++ ) {
      // extract the previous and the current element in the beamline
      const Element::ElementBase *prev_elem = *( it-1 ), *elem = *( it );
      if ( s_max>0 and elem->s()>s_max ) return false;

      const Aperture::ApertureBase* aper = prev_elem->aperture();
      if ( aper and aper->type()!=Aperture::anInvalidAperture ) {

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
    const StateVector shift( elem->position(), elem->angles(), 0., 0. );

    /*std::ostringstream os1, os2;
    os1 << elem->type(); os2 << elem->matrix( eloss, ini_pos.stateVector().m(), qp );
    PrintInfo( Form( "Propagating through %s element \"%s\" with transfer matrix\n%s", os1.str().c_str(), elem->name().c_str(), os2.str().c_str() ) );*/

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

