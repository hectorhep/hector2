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
      void computeMatrix( float, float, int );

    private:
  };
}

#endif
