#include "ApertureBase.h"

namespace Hector
{
  namespace Aperture
  {
    ApertureBase::ApertureBase( const Type& type, const CLHEP::Hep2Vector& pos, float x1, float x2, float x3, float x4 ) :
      type_( type ), pos_( pos ),
      x1_( x1 ), x2_( x2 ), x3_( x3 ), x4_( x4 ) // shape parameters
    {
    }

    ApertureBase::~ApertureBase()
    {
    }

    std::ostream&
    operator<<( std::ostream& os, const ApertureBase::Type& type )
    {
      switch ( type ) {
        case ApertureBase::None: os << "none"; break;
        case ApertureBase::Rectangular: os << "rectangular"; break;
        case ApertureBase::Circular: os << "circular"; break;
        case ApertureBase::Elliptic: os << "elliptic"; break;
        case ApertureBase::RectElliptic: os << "rect-elliptic"; break;
      }
      return os;
    }

    std::ostream&
    operator<<( std::ostream& os, const ApertureBase& ap )
    {
      os << ap.type() << " aperture"
         << " (param. (" << ap.x1() << ", " << ap.x2() << ", " << ap.x3() << ", " << ap.x4() << "),"
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
