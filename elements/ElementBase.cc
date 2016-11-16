#include "ElementBase.h"

namespace Element
{
  ElementBase::ElementBase( const Type& type, const char* name ) :
    type_( type ), name_( name )
  {
  }

  ElementBase::~ElementBase()
  {}

  ElementBase*
  ElementBase::clone() const
  {
    ElementBase* elem = new ElementBase( type_, name_ );
    elem->setS( s_ );
    elem->setX( xpos_ );
    elem->setY( ypos_ );
    elem->setTx( tx_ );
    elem->setTy( ty_ );
    elem->setLength( length_ );
    elem->setMagneticStrength( magnetic_strength_ );
    return elem;
  }

}
