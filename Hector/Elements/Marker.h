#ifndef Hector_Elements_Marker_h
#define Hector_Elements_Marker_h

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
        Marker( const std::string& name, float spos, float length );

        std::shared_ptr<ElementBase> clone() const override { return std::make_shared<Marker>( *this ); }
        /** \note \f$ \mathbf{M} = \mathbf{I}_6 \f$
         */
        Matrix matrix( float, float, int ) const override;
    };
  }
}

#endif
