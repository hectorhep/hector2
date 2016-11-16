#include "ApertureBase.h"

namespace Element
{
  ApertureBase::ApertureBase( const Type& type, const CLHEP::Hep2Vector& pos, float x1, float x2, float x3, float x4 ) :
    type_( type ), pos_( pos ),
    x1_( x1 ), x2_( x2 ), x3_( x3 ), x4_( x4 ) // shape parameters
  {
  }

  ApertureBase::~ApertureBase()
  {
  }

  ApertureBase*
  ApertureBase::clone() const
  {
    return new ApertureBase( type_, pos_, x1_, x2_, x3_, x4_ );
  }

  bool
  ApertureBase::contains( const CLHEP::Hep2Vector& ) const
  {
    return false;
  }
}
