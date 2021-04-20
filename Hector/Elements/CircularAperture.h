#ifndef Hector_Elements_CircularAperture_h
#define Hector_Elements_CircularAperture_h

#include "EllipticAperture.h"

namespace hector {
  namespace aperture {
    /// Round shape aperture
    class CircularAperture : public EllipticAperture {
    public:
      /// Class constructor
      CircularAperture(double, const TwoVector& pos = TwoVector());
      ~CircularAperture();

      std::shared_ptr<ApertureBase> clone() const override { return std::make_shared<CircularAperture>(*this); }
    };
  }  // namespace aperture
}  // namespace hector

#endif
