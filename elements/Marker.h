#ifndef Elements_Marker_h
#define Elements_Marker_h

#include "ElementBase.h"

namespace Hector
{
  namespace Element
  {
    class Marker : public ElementBase
    {
      public:
        Marker( const std::string& name );

        Marker* clone() const {
          Marker* out = new Marker( *this );
          if ( aperture_ ) out->setAperture( aperture_->clone() );
          return out;
        }
        CLHEP::HepMatrix matrix( float, float, int ) const;

      private:

    };
  }
}

#endif
