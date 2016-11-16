#ifndef Elements_RectangularAperture
#define Elements_RectangularAperture

#include "ApertureBase.h"

namespace Element
{
  class RectangularAperture : public ApertureBase
  {
    public:
      RectangularAperture( float, float, const CLHEP::Hep2Vector& pos=CLHEP::Hep2Vector() );
      ~RectangularAperture();

      bool contains( const CLHEP::Hep2Vector& ) const;

  };
}

#endif