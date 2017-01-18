#ifndef Elements_Drift_h
#define Elements_Drift_h

#include "ElementBase.h"

namespace Hector
{
  namespace Element
  {
    class Drift : public ElementBase
    {
      public:
        Drift( const ElementBase::Type&, const std::string& );
        ~Drift();

        Drift* clone() const {
          Drift* out = new Drift( *this );
          if ( aperture_ ) out->setAperture( aperture_->clone() );
          return out;
        }
        CLHEP::HepMatrix matrix( float, float, int ) const;

      private:
    };
  }
}

#endif
