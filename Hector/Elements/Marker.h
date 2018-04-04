#ifndef Hector_Elements_Marker_h
#define Hector_Elements_Marker_h

#include "Drift.h"

namespace Hector
{
  namespace Element
  {
    /// Marker object builder
    class Marker : public Drift
    {
      public:
        /// Class constructor
        Marker( const std::string& name, double spos, double length = 0., const Type& type = aMarker ) :
          Drift( name, type, spos, length ) {}

        std::shared_ptr<ElementBase> clone() const override { return std::make_shared<Marker>( *this ); }
    };
  }
}

#endif
