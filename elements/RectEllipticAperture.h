#ifndef Elements_RectEllipticAperture
#define Elements_RectEllipticAperture

#include "ApertureBase.h"

namespace Element
{
  class RectEllipticAperture : public ApertureBase
  {
    public:
      RectEllipticAperture( float, float, float, float, const CLHEP::Hep2Vector& pos=CLHEP::Hep2Vector() );
      ~RectEllipticAperture();

      bool contains( const CLHEP::Hep2Vector& ) const;

  };
}

#endif
