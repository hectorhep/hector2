#ifndef Elements_CircularAperture_h
#define Elements_CircularAperture_h

#include "EllipticAperture.h"

namespace Element
{
  class CircularAperture : public EllipticAperture
  {
    public:
      CircularAperture( float, const CLHEP::Hep2Vector& pos=CLHEP::Hep2Vector() );
      ~CircularAperture();

      CircularAperture* clone() const { return new CircularAperture( *this ); }

    private:
  };
}

#endif
