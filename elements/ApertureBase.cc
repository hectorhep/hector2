#include "ApertureBase.h"

namespace Hector
{
  namespace Aperture
  {
    ApertureBase::ApertureBase( const Type& type, const CLHEP::Hep2Vector& pos, const std::vector<float>& param ) :
      type_( type ), pos_( pos ), param_( param )
    {}

    ApertureBase::~ApertureBase()
    {
      param_.clear();
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
