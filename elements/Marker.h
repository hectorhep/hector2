#ifndef Elements_Marker_h
#define Elements_Marker_h

#include "ElementBase.h"

namespace Hector
{
  namespace Element
  {
    /// Marker object builder
    class Marker : public ElementBase
    {
      public:
        /// Class constructor
        Marker( const std::string& name );

        Marker* clone() const { return new Marker( *this ); }
        /** \note \f$ \mathbf{M} = \mathbf{I}_6 \f$
         */
        CLHEP::HepMatrix matrix( float, float, int ) const;

      private:

    };
  }
}

#endif
