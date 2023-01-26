#ifndef Hector_Apertures_ApertureFwd_h
#define Hector_Apertures_ApertureFwd_h

#include <memory>
#include <vector>

namespace hector {
  /// Collection of beamline elements apertures
  namespace aperture {
    class Aperture;
    /// A smart pointer to a beamline element aperture
    typedef std::shared_ptr<aperture::Aperture> AperturePtr;
    /// List of apertures
    typedef std::vector<AperturePtr> Apertures;
  }  // namespace aperture
}  // namespace hector

#endif
