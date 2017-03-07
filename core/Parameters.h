#ifndef Core_Constants_h
#define Core_Constants_h

/// Mother of all namespaces/objects in Hector
/// \author Laurent Forthomme <laurent.forthomme@cernSPAMNOT.ch>
/// \date Nov 2016
namespace Hector
{
  /// List of user-parameterisable constants for the run
  /// \note All parameters are to be left untouched once the run is started
  struct Parameters
  {
    /// Energy of the primary particles in the beam (in GeV)
    static float beam_energy;
    /// Mass of the primary particles in the beam (in GeV/c2)
    static float beam_particles_mass;
    /// Crossing angle of the particles beams (in rad)
    static float crossing_angle;
    /// Electric charge of the primary particles in the beam (in e)
    static int beam_particles_charge;
    /// Do we use the relative energy loss in the path computation through elements?
    static bool use_relative_energy;
    /// Are the elements overlaps to be corrected inside a beamline
    static bool correct_beamline_overlaps;
  };
}

#endif
