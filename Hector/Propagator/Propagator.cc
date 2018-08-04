#include "Hector/Propagator/Propagator.h"

#include "Hector/Beamline/Beamline.h"
#include "Hector/Elements/ElementBase.h"

#include "Hector/Core/Exception.h"
#include "Hector/Core/ParticleStoppedException.h"

#include "Hector/Utils/Utils.h"

#include <sstream>

namespace Hector
{
  void
  Propagator::propagate( Particle& part, double s_max ) const
  {
    part.clear();

    const double energy_loss = ( Parameters::get()->useRelativeEnergy() )
      ? Parameters::get()->beamEnergy()-part.lastStateVector().energy()
      : part.lastStateVector().energy();

    const double first_s = part.firstS();

    try {

      for ( Elements::const_iterator it = beamline_->begin()+1; it != beamline_->end(); ++it ) {
        // extract the previous and the current element in the beamline
        const auto prev_elem = *( it-1 ), elem = *it;
        if ( elem->s() > s_max ) break;

        const Particle::Position in_pos( *part.rbegin() );

        // initialise the outwards position
        Particle::Position out_pos( -1., StateVector() );

        // between two elements
        if ( first_s > prev_elem->s() && first_s < elem->s() ) {
          switch ( prev_elem->type() ) {
            case Element::aDrift:
              PrintInfo( Form( "Path starts inside element %s",
                               prev_elem->name().c_str() ) );
              break;
            default:
              PrintInfo( Form( "Path starts inside drift %s",
                               prev_elem->name().c_str() ) );
              break;
          }

          // build a temporary element mimicking the drift effect
          auto elem_tmp = prev_elem->clone();
          elem_tmp->setS( first_s );
          elem_tmp->setLength( elem->s()-first_s );
          out_pos = propagateThrough( in_pos, elem_tmp, energy_loss, part.charge() );
        }
        // before one element
        if ( first_s <= elem->s() )
          out_pos = propagateThrough( in_pos, elem, energy_loss, part.charge() );

        if ( out_pos.s() < 0. )
          continue; // no new point to add to the particle's trajectory

        part.addPosition( out_pos.s(), out_pos.stateVector() );

        if ( !Parameters::get()->computeApertureAcceptance() )
          continue;

        const auto& aper = prev_elem->aperture();
        if ( !aper || aper->type() != Aperture::anInvalidAperture )
          continue;

        const TwoVector pos_prev_elem( part.stateVectorAt( prev_elem->s() ).position() );
        if ( !aper->contains( pos_prev_elem ) ) {
          std::ostringstream os1, os2;
          os1 << pos_prev_elem;
          os2 << aper->position();
          throw ParticleStoppedException( __PRETTY_FUNCTION__, prev_elem.get(), JustWarning,
            Form( "Entering at %s, s = %.2f m\n\t"
                  "Aperture centre at %s\n\t"
                  "Distance to aperture centre: %.2f cm",
                  os1.str().c_str(), prev_elem->s(),
                  os2.str().c_str(),
                  ( aper->position()-pos_prev_elem ).mag()*1.e2 ).c_str() );
        }
        // has passed through the element?
        //std::cout << prev_elem->s()+prev_elem->length() << "\t" << part.stateVectorAt( prev_elem->s()+prev_elem->length() ).position() << std::endl;
        if ( !aper->contains( part.stateVectorAt( prev_elem->s()+prev_elem->length() ).position() ) )
          throw ParticleStoppedException( __PRETTY_FUNCTION__, prev_elem.get(), JustWarning,
                                            Form( "Did not pass aperture %s", aper->typeName().c_str() ).c_str() );
      }
    }
    catch ( const ParticleStoppedException& ) { throw; }
    catch ( const Exception& ) { throw; }

    // finish by sorting all positions according to their s-coordinate
    //part.sortPositions();
  }

  bool
  Propagator::stopped( Particle& part, double s_max ) const
  {
    for ( Elements::const_iterator it = beamline_->begin()+1; it != beamline_->end(); ++it ) {
      // extract the previous and the current element in the beamline
      const auto prev_elem = *( it-1 ), elem = *it;
      if ( s_max>0 && elem->s() > s_max )
        return false;

      const auto& aper = prev_elem->aperture();
      if ( aper && aper->type() != Aperture::anInvalidAperture ) {
        // has passed the element entrance?
        if ( !aper->contains( part.stateVectorAt( prev_elem->s() ).position() ) )
          return true;
        // has passed through the element?
        if ( !aper->contains( part.stateVectorAt( prev_elem->s()+prev_elem->length() ).position() ) )
          return true;
      }
    }
    return false;
  }

  Particle::Position
  Propagator::propagateThrough( const Particle::Position& ini_pos, const std::shared_ptr<Element::ElementBase> elem, double eloss, int qp ) const
  {
    try {
      //const StateVector shift( elem->relativePosition(), elem->angles(), 0., 0. );
      //const StateVector shift( elem->relativePosition(), TwoVector(), 0., 0. );
      const StateVector shift( TwoVector(), TwoVector(), 0., 0. );
      const Vector prop = elem->matrix( eloss, ini_pos.stateVector().m(), qp ) * ( ini_pos.stateVector().vector()-shift.vector() ) + shift.vector();

      if ( Parameters::get()->loggingThreshold() >= Debug ) {
        std::ostringstream os1; os1 << ini_pos.stateVector().vector().T();
        std::ostringstream os2; os2 << elem->matrix( eloss, ini_pos.stateVector().m(), qp );
        std::ostringstream os3; os3 << prop.T();
        PrintDebug( Form( "Propagating particle of mass %g GeV and state vector at s = %g m:%s\t"
                          "through %s element \"%s\" "
                          "at s = %g m, "
                          "of length %g m,\n\t"
                          "and with transfer matrix:"
                          "%s\t"
                          "Resulting state vector:%s",
                          ini_pos.stateVector().m(), ini_pos.s(), os1.str().c_str(),
                          elem->typeName().c_str(), elem->name().c_str(), elem->s(), elem->length(),
                          os2.str().c_str(), os3.str().c_str() ) );
      }

      // perform the propagation (assuming that mass is conserved...)
      StateVector vec( prop, ini_pos.stateVector().m() );

      // convert the angles -> tan-1( angle )
      //const TwoVector ang_old = vec.angles();
      //vec.setAngles( math::atan2( ang_old ) );

      return Particle::Position( elem->s()+elem->length(), vec );
    } catch ( Exception& e ) { throw; }
  }

  void
  Propagator::propagate( Particles& beam, double s_max ) const
  {
    for ( auto& part : beam )
      propagate( part, s_max );
  }
}

