#ifndef Hector_Elements_EllipticAperture
#define Hector_Elements_EllipticAperture

#include "ApertureBase.h"

namespace Hector {
  namespace Aperture {
    /// Elliptic shape aperture
    class EllipticAperture : public ApertureBase {
    public:
      /// Class constructor
      EllipticAperture(double, double, const TwoVector& pos = TwoVector());
      ~EllipticAperture();

      std::shared_ptr<ApertureBase> clone() const override { return std::make_shared<EllipticAperture>(*this); }

      bool contains(const TwoVector&) const override;
      TwoVector limits() const override;
    };
  }  // namespace Aperture
}  // namespace Hector

#endif
