#ifndef Core_Constants_h
#define Core_Constants_h

/// Mother of all namespaces/objects in Hector
namespace Hector
{
  /// List of constants to be left untouched once the run is started
  struct Constants
  {
    /// Energy of the primary particles in the beam (in GeV)
    static float beam_energy;
    /// Mass of the primary particles in the beam (in GeV/c2)
    static float beam_particles_mass;
    /// Crossing angle of the particles beams (in urad)
    static float crossing_angle;
    /// Electric charge of the primary particles in the beam (in e)
    static int beam_particles_charge;
    /// Do we use the relative energy loss in the path computation through elements?
    static bool use_relative_energy;
  };
}

#endif
