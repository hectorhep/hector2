#ifndef Hector_Elements_RectangularAperture
#define Hector_Elements_RectangularAperture

#include "ApertureBase.h"

namespace hector {
  namespace aperture {
    /// Rectangular shape aperture
    class RectangularAperture : public ApertureBase {
    public:
      /// Class constructor
      RectangularAperture(double, double, const TwoVector& pos = TwoVector());
      ~RectangularAperture();

      std::shared_ptr<ApertureBase> clone() const override { return std::make_shared<RectangularAperture>(*this); }

      bool contains(const TwoVector&) const override;
      TwoVector limits() const override;
    };
  }  // namespace aperture
}  // namespace hector

#endif
