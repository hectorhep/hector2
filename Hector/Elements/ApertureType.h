#ifndef Hector_Elements_ApertureType_h
#define Hector_Elements_ApertureType_h

#include <iostream>

namespace Hector
{
  /// Collection of apertures to be associated to element objects
  namespace Aperture
  {
    /// List of types allowed for an aperture
    typedef enum {
      anInvalidAperture,
      aRectangularAperture,
      anEllipticAperture,
      aCircularAperture,
      aRectEllipticAperture,
      aRectCircularAperture,
      aRaceTrackAperture,
      anOctagonalAperture
    } Type;
    /// Human-readable printout of an aperture type
    std::ostream& operator<<( std::ostream&, const Type& );
  }
}

#endif
