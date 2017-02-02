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
        /// Class constructor
        Drift( const ElementBase::Type&, const std::string& );
        ~Drift();

        Drift* clone() const {
          Drift* out = new Drift( *this );
          if ( aperture_ ) out->setAperture( aperture_->clone() );
          return out;
        }
        /** \note \f$
          \mathbf{M} = \left(
          \begin{array}{cccccc}
          1 & 0 & 0 & 0 & 0 & 0\\
          l & 1 & 0 & 0 & 0 & 0\\
          0 & 0 & 1 & 0 & 0 & 0\\
          0 & 0 & l & 1 & 0 & 0\\
          0 & 0 & 0 & 0 & 1 & 0\\
          0 & 0 & 0 & 0 & 0 & 1\\
          \end{array}
          \right)
          \f$
        */
        CLHEP::HepMatrix matrix( float eloss=-1., float mp=-1., int qp=0 ) const;
        /// Build a transfer matrix for a given drift length
        /// \param[in] length drift length
        static CLHEP::HepMatrix genericMatrix( float length );

      private:
    };
  }
}

#endif
