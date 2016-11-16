#include "ElementBase.h"

namespace Element
{
  ElementBase::ElementBase( const Type& type, const char* name ) :
    type_( type ), name_( name ), aperture_( 0 )
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

}
