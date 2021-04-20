#ifndef Hector_Elements_ApertureType_h
#define Hector_Elements_ApertureType_h

#include <iostream>

namespace hector {
  /// Collection of apertures to be associated to element objects
  namespace aperture {
    /// List of types allowed for an aperture
    enum Type {
      anInvalidAperture = -1,
      aRectangularAperture,
      anEllipticAperture,
      aCircularAperture,
      aRectEllipticAperture,
      aRectCircularAperture,
      aRaceTrackAperture,
      anOctagonalAperture
    };
  }  // namespace aperture
  /// Human-readable printout of an aperture type
  std::ostream& operator<<(std::ostream&, const aperture::Type&);
}  // namespace hector

#endif
