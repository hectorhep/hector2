#ifndef Hector_Apertures_RectElliptic_h
#define Hector_Apertures_RectElliptic_h

#include "Hector/Apertures/ApertureBase.h"

namespace hector {
  namespace aperture {
    /// Rectangular/elliptic shape aperture
    class RectElliptic : public ApertureBase {
    public:
      /// Class constructor
      RectElliptic(double, double, double, double, const TwoVector& pos = TwoVector());
      ~RectElliptic();

      std::shared_ptr<ApertureBase> clone() const override { return std::make_shared<RectElliptic>(*this); }

      bool contains(const TwoVector&) const override;
      TwoVector limits() const override;
    };
  }  // namespace aperture
}  // namespace hector

#endif
