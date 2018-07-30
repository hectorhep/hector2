#ifndef Hector_Elements_Collimator_h
#define Hector_Elements_Collimator_h

#include "Drift.h"

namespace Hector
{
  namespace Element
  {
    /// Collimator object builder
    /// \note Treated as a simple drift with a finite aperture opening
    class Collimator : public Drift
    {
      public:
        /// Class constructor
        Collimator( const std::string& name, double spos = 0., double length = 0. );

        std::shared_ptr<ElementBase> clone() const override { return std::make_shared<Collimator>( *this ); }
    };
  }
}

#endif
