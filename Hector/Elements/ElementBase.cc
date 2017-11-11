#include "Hector/Elements/ElementBase.h"
#include "Hector/Core/Utils.h"
#include "Hector/Core/Parameters.h"

#include <CLHEP/Matrix/DiagMatrix.h>
#include <CLHEP/Vector/TwoVector.h>

#include <sstream>

namespace Hector
{
  namespace Element
  {
    ElementBase::ElementBase( const Type& type, const std::string& name, float spos, float length ) :
      type_( type ), name_( name ),
      length_( length ), magnetic_strength_( 0. ), s_( spos )
    {}

    ElementBase::ElementBase( ElementBase& rhs ) :
      type_( rhs.type_ ), name_( rhs.name_ ),
      aperture_( rhs.aperture_ ),
      length_( rhs.length_ ), magnetic_strength_( rhs.magnetic_strength_ ),
      pos_( rhs.pos_ ), angles_( rhs.angles_ ), s_( rhs.s_ ),
      beta_( rhs.beta_ ), disp_( rhs.disp_ ), rel_pos_( rhs.rel_pos_ )
    {}

    ElementBase::ElementBase( const ElementBase& rhs ) :
      type_( rhs.type_ ), name_( rhs.name_ ),
      aperture_( rhs.aperture_ ),
      length_( rhs.length_ ), magnetic_strength_( rhs.magnetic_strength_ ),
      pos_( rhs.pos_ ), angles_( rhs.angles_ ), s_( rhs.s_ ),
      beta_( rhs.beta_ ), disp_( rhs.disp_ ), rel_pos_( rhs.rel_pos_ )
    {}

    bool
    ElementBase::operator==( const ElementBase& rhs ) const
    {
      if ( type_ != rhs.type_ ) return false;
      if ( s_ != rhs.s_ ) return false;
      if ( pos_ != rhs.pos_ ) return false;
      if ( magnetic_strength_ != rhs.magnetic_strength_ ) return false;
      if ( length_ != rhs.length_ ) return false;
      if ( name_ != rhs.name_ ) return false;
      if ( aperture_ ) {
        if ( !rhs.aperture_ ) return false;
        if ( *aperture_ != *rhs.aperture_ ) return false;
      }
      return true;
    }

    void
    ElementBase::setAperture( const std::shared_ptr<Aperture::ApertureBase> apert )
    {
      aperture_ = apert;
    }

    float
    ElementBase::fieldStrength( float eloss, float mp, int qp ) const
    {
      // only act on charged particles
      if ( qp == 0 ) return 0.;

      // reweight the field strength by the particle charge and momentum
      const float eini = Parameters::get()->beamEnergy(),
                  mp0 = Parameters::get()->beamParticlesMass(),
                  e = eini-eloss;
      const float p0 = sqrt( ( eini-mp0 )*( eini+mp0 ) ), // e_ini^2 - p_0^2 = mp0^2
                  p = sqrt( ( e-mp )*( e+mp ) ); // e^2 - p^2 = mp^2
      return magnetic_strength_*( p0/p )*( qp/Parameters::get()->beamParticlesCharge() );
    }
  }
  /// Human-readable printout of a beamline element object
  std::ostream&
  operator<<( std::ostream& os, const Element::ElementBase& elem )
  {
    std::ostringstream oss; oss << elem.type();
    os << Form( "%15s %17s (length = %5.2f m) at %6.2f < s < %6.2f m", oss.str().c_str(), elem.name().c_str(), elem.length(), elem.s(), elem.s()+elem.length() );
    if ( elem.aperture() )
      return os << " with aperture: " << elem.aperture();
    return os << " with no aperture restriction";
  }

  /// Human-readable printout of a beamline element object
  std::ostream&
  operator<<( std::ostream& os, const Element::ElementBase* elem )
  {
    return os << *( elem );
  }
}
