#ifndef Hector_Apertures_Elliptic_h
#define Hector_Apertures_Elliptic_h

#include "Hector/Apertures/ApertureBase.h"

namespace hector {
  namespace aperture {
    /// Elliptic shape aperture
    class Elliptic : public ApertureBase {
    public:
      /// Class constructor
      Elliptic(double, double, const TwoVector& pos = TwoVector());
      ~Elliptic();

      std::shared_ptr<ApertureBase> clone() const override { return std::make_shared<Elliptic>(*this); }

      bool contains(const TwoVector&) const override;
      TwoVector limits() const override;
    };
  }  // namespace aperture
}  // namespace hector

#endif
