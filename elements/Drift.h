#ifndef Elements_Drift_h
#define Elements_Drift_h

#include "ElementBase.h"

namespace Element
{
  class Drift : public ElementBase
  {
    public:
      Drift( const ElementBase::Type&, const std::string& );
      ~Drift();

      Drift* clone() const { return new Drift( *this ); }
      CLHEP::HepMatrix matrix( float, float, int ) const;

    private:
  };
}

#endif
