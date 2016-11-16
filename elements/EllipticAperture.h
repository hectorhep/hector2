#ifndef Elements_EllipticAperture
#define Elements_EllipticAperture

#include "ApertureBase.h"

namespace Element
{
  class EllipticAperture : public ApertureBase
  {
    public:
      EllipticAperture( float, float, const CLHEP::Hep2Vector& pos=CLHEP::Hep2Vector() );
      ~EllipticAperture();

      bool contains( const CLHEP::Hep2Vector& ) const;

  };
}

#endif
