#include "ElementBase.h"

namespace Element
{
  ElementBase::ElementBase( const Type& type, const std::string& name ) :
    type_( type ), name_( name ), aperture_( 0 ),
    length_( 0. ), magnetic_strength_( 0. ), s_( 0. ), tx_( 0. ), ty_( 0. )
  {
  }

  ElementBase::~ElementBase()
  {
    if ( aperture_ ) delete aperture_;
  }

  ElementBase*
  ElementBase::clone() const
  {
    ElementBase* out = new ElementBase( type_, name_ );
    out->setS( s_ );
    out->setPosition( pos_ );
    out->setTx( tx_ );
    out->setTy( ty_ );
    out->setLength( length_ );
    out->setMagneticStrength( magnetic_strength_ );
    if ( aperture_ ) out->setAperture( aperture_ );
    return out;
  }

  std::ostream&
  operator<<( std::ostream& os, const ElementBase::Type& type )
  {
    switch ( type ) {
      case ElementBase::Invalid: os << "invalid"; break;
      case ElementBase::Marker: os << "marker"; break;
      case ElementBase::Drift: os << "drift"; break;
      case ElementBase::Monitor: os << "monitor"; break;
      case ElementBase::RectangularDipole: os << "rectangular dipole"; break;
      case ElementBase::SectorDipole: os << "sector dipole"; break;
      case ElementBase::Quadrupole: os << "quadrupole"; break;
      case ElementBase::Sextupole: os << "sextupole"; break;
      case ElementBase::Multipole: os << "multipole"; break;
      case ElementBase::VerticalKicker: os << "vertical kicker"; break;
      case ElementBase::HorizontalKicker: os << "horizontal kicker"; break;
      case ElementBase::RectangularCollimator: os << "rectangular collimator"; break;
      case ElementBase::EllipticalCollimator: os << "elliptical collimator"; break;
      case ElementBase::CircularCollimator: os << "circular collimator"; break;
      case ElementBase::Placeholder: os << "placeholder"; break;
      case ElementBase::Instrument: os << "instrument"; break;
    }
    return os;
  }

  std::ostream&
  operator<<( std::ostream& os, const ElementBase& elem )
  {
    os << "Element (" << elem.type() << ") at s=" << elem.s();
    return os;
  }

  std::ostream&
  operator<<( std::ostream& os, const ElementBase* elem )
  {
    os << *( elem );
    return os;
  }
}
