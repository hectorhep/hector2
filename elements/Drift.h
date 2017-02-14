#ifndef Elements_Drift_h
#define Elements_Drift_h

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
        Drift( const std::string&, const ElementBase::Type& type=ElementBase::Drift );

        Drift* clone() const { return new Drift( *this ); }
        CLHEP::HepMatrix matrix( float eloss=-1., float mp=-1., int qp=0 ) const {
          return genericMatrix( length_ );
        }
        /// Build a transfer matrix for a given drift length
        /// \param[in] length drift length
        /** \note \f$
         * \mathbf{M} = \left(
         * \begin{array}{cccccc}
         * 1 & 0 & 0 & 0 & 0 & 0\\
         * l & 1 & 0 & 0 & 0 & 0\\
         * 0 & 0 & 1 & 0 & 0 & 0\\
         * 0 & 0 & l & 1 & 0 & 0\\
         * 0 & 0 & 0 & 0 & 1 & 0\\
         * 0 & 0 & 0 & 0 & 0 & 1\\
         * \end{array}
         * \right)
         * \f$
         */
        static CLHEP::HepMatrix genericMatrix( float length );

      private:
    };
  }
}

#endif
