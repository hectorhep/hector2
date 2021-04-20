#ifndef Hector_Apertures_Rectangular_h
#define Hector_Apertures_Rectangular_h

#include "Hector/Apertures/ApertureBase.h"

namespace hector {
  namespace aperture {
    /// Rectangular shape aperture
    class Rectangular : public ApertureBase {
    public:
      /// Class constructor
      Rectangular(double, double, const TwoVector& pos = TwoVector());
      ~Rectangular();

      std::shared_ptr<ApertureBase> clone() const override { return std::make_shared<Rectangular>(*this); }

      bool contains(const TwoVector&) const override;
      TwoVector limits() const override;
    };
  }  // namespace aperture
}  // namespace hector

#endif
