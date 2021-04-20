#ifndef Hector_Apertures_Circular_h
#define Hector_Apertures_Circular_h

#include "Hector/Apertures/Elliptic.h"

namespace hector {
  namespace aperture {
    /// Round shape aperture
    class Circular : public Elliptic {
    public:
      /// Class constructor
      Circular(double, const TwoVector& pos = TwoVector());
      ~Circular();

      std::shared_ptr<ApertureBase> clone() const override { return std::make_shared<Circular>(*this); }
    };
  }  // namespace aperture
}  // namespace hector

#endif
