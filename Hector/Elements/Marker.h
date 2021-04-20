#ifndef Hector_Elements_Marker_h
#define Hector_Elements_Marker_h

#include "Drift.h"

namespace hector {
  namespace element {
    /// Marker object builder
    class Marker : public Drift {
    public:
      /// Class constructor
      Marker(const std::string& name, double spos, double length = 0.) : Drift(name, aMarker, spos, length) {}

      std::shared_ptr<ElementBase> clone() const override { return std::make_shared<Marker>(*this); }
    };
  }  // namespace element
}  // namespace hector

#endif
