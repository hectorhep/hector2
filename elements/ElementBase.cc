#include "ElementBase.h"

namespace Hector
{
  namespace Element
  {
    ElementBase::ElementBase( const Type& type, const std::string& name, float spos, float length ) :
      type_( type ), name_( name ), aperture_( 0 ),
      length_( length ), magnetic_strength_( 0. ), s_( spos )
    {}

    ElementBase::ElementBase( const ElementBase& rhs ) :
      type_( rhs.type_ ), name_( rhs.name_ ), aperture_( 0 ),
      length_( rhs.length_ ), magnetic_strength_( rhs.magnetic_strength_ ),
      pos_( rhs.pos_ ), angles_( rhs.angles_ ), s_( rhs.s_ ),
      beta_( rhs.beta_ ), disp_( rhs.disp_ ), rel_pos_( rhs.rel_pos_ )
    {
      if ( rhs.aperture_ ) aperture_ = rhs.aperture_->clone();
    }

    ElementBase::~ElementBase()
    {
      if ( aperture_!=0 ) delete aperture_;
    }

    bool
    ElementBase::operator==( const ElementBase& rhs ) const
    {
      if ( type_!=rhs.type_ ) return false;
      if ( s_!=rhs.s_ ) return false;
      if ( pos_!=rhs.pos_ ) return false;
      if ( magnetic_strength_!=rhs.magnetic_strength_ ) return false;
      if ( length_!=rhs.length_ ) return false;
      if ( name_!=rhs.name_ ) return false;
      if ( aperture_ and rhs.aperture_ and *aperture_!=*rhs.aperture_ ) return false;
      return true;
    }

    float
    ElementBase::fieldStrength( float eloss, float mp, int qp ) const
    {
      // only act on charged particles
      if ( qp==0 ) return 0.;

      // reweight the field strength by the particle charge and momentum
      const float eini = Parameters::beam_energy,
                  mp0 = Parameters::beam_particles_mass,
                  e = eini-eloss;
      const float p0 = sqrt( ( eini-mp0 )*( eini+mp0 ) ), // e_ini^2 - p_0^2 = mp0^2
                  p = sqrt( ( e-mp )*( e+mp ) ); // e^2 - p^2 = mp^2
      return magnetic_strength_*( p0/p )*( qp/Parameters::beam_particles_charge );
    }

    /// Human-readable printout of a beamline element object
    std::ostream&
    operator<<( std::ostream& os, const ElementBase& elem )
    {
      std::ostringstream oss; oss << elem.type();
      os << Form( "{%15s} %17s (length = %5.2f m) at %6.2f < s < %6.2f m", oss.str().c_str(), elem.name().c_str(), elem.length(), elem.s(), elem.s()+elem.length() );
      if ( elem.aperture() ) { os << "\twith aperture: " << elem.aperture(); }
      return os;
    }

    /// Human-readable printout of a beamline element object
    std::ostream&
    operator<<( std::ostream& os, const ElementBase* elem )
    {
      return os << *( elem );
    }
    /// Human-readable printout of a beamline element type
    std::ostream&
    operator<<( std::ostream& os, const Type& type )
    {
      switch ( type ) {
        case anInvalidElement: os << "invalid"; break;
        case aMarker: os << "marker"; break;
        case aDrift: os << "drift"; break;
        case aMonitor: os << "monitor"; break;
        case aRectangularDipole: os << "rect.dipole"; break;
        case aSectorDipole: os << "sector dipole"; break;
        case aGenericQuadrupole: os << "quadrupole"; break;
        case anHorizontalQuadrupole: os << "hor.quadrupole"; break;
        case aVerticalQuadrupole: os << "vert.quadrupole"; break;
        case aSextupole: os << "sextupole"; break;
        case aMultipole: os << "multipole"; break;
        case aVerticalKicker: os << "vertic.kicker"; break;
        case anHorizontalKicker: os << "horiz.kicker"; break;
        case aRectangularCollimator: os << "rect.collimator"; break;
        case anEllipticalCollimator: os << "ellip.collimator"; break;
        case aCircularCollimator: os << "circular collimator"; break;
        case aPlaceholder: os << "placeholder"; break;
        case anInstrument: os << "instrument"; break;
        case aSolenoid: os << "solenoid"; break;
      }
      return os;
    }
  }
}
