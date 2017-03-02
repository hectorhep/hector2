#include "ApertureBase.h"

namespace Hector
{
  namespace Aperture
  {
    ApertureBase::ApertureBase( const Type& type, const CLHEP::Hep2Vector& pos, const Parameters& param ) :
      type_( type ), pos_( pos ), param_( param )
    {}

    ApertureBase::~ApertureBase()
    {
      param_.clear();
    }

    bool
    ApertureBase::operator==( const ApertureBase& rhs ) const
    {
      if ( type_!=rhs.type_ ) return false;
      if ( pos_!=rhs.pos_ ) return false;
      if ( param_!=rhs.param_ ) return false;
      return true;
    }

    std::ostream&
    operator<<( std::ostream& os, const Type& type )
    {
      switch ( type ) {
        case anInvalidType: os << "invalid"; break;
        case aRectangularAperture: os << "rectangular"; break;
        case aCircularAperture: os << "circular"; break;
        case anEllipticAperture: os << "elliptic"; break;
        case aRectEllipticAperture: os << "rect-elliptic"; break;
      }
      return os;
    }

    std::ostream&
    operator<<( std::ostream& os, const ApertureBase& ap )
    {
      os << ap.type()
         << Form( " (param. (%.4f, %.4f, %.4f, %.4f),", ap.p( 0 ), ap.p( 1 ), ap.p( 2 ), ap.p( 3 ) )
         << " center at " << ap.position() << ")";
      return os;
    }

    std::ostream&
    operator<<( std::ostream& os, const ApertureBase* ap )
    {
      os << *( ap );
      return os;
    }
  }
}
