#ifndef Hector_Elements_Drift_h
#define Hector_Elements_Drift_h

#include "ElementBase.h"

namespace Hector
{
  namespace Element
  {
    /// Drift object builder
    class Drift : public ElementBase
    {
      public:
        /// Class constructor
        Drift( const std::string&, float spos=0., float length=0. );
        /// Class constructor
        Drift( const std::string&, const Type& type, float spos=0., float length=0. );

        Drift* clone() const { return new Drift( *this ); }
        CLHEP::HepMatrix matrix( float eloss=-1., float mp=-1., int qp=0 ) const;
        /// Build a transfer matrix for a given drift length
        /// \param[in] length drift length
        /** \note \f$
         * \mathbf{M} = \left(
         * \begin{array}{cccccc}
         * 1 & L & 0 & 0 & 0 & 0\\
         * 0 & 1 & 0 & 0 & 0 & 0\\
         * 0 & 0 & 1 & L & 0 & 0\\
         * 0 & 0 & 0 & 1 & 0 & 0\\
         * 0 & 0 & 0 & 0 & 1 & 0\\
         * 0 & 0 & 0 & 0 & 0 & 1\\
         * \end{array}
         * \right)
         * \f$
         */
        static CLHEP::HepMatrix genericMatrix( float length );
    };
  }
}

#endif
