#ifndef Hector_Utils_Utils_h
#define Hector_Utils_Utils_h

#include "Hector/Core/Parameters.h"

#include <string>

namespace Hector
{
  class Particle;
  /// Format a string using a printf style format descriptor.
  std::string Form( const std::string fmt, ... );
  /// Remove leading and trailing whitespaces to a string
  std::string trim( const std::string& str );
  /// Get the lowercase version of a string
  std::string lowercase( const std::string& str );
  /// Let the particle emit a photon
  void emitGamma( Particle& part_in, double e_gamma, double q2_gamma, double phi_min, double phi_max );
  /// Convert a particle energy to its momentum loss
  double e_to_xi( double energy, double e0 = Parameters::get()->beamEnergy() );
  /// Convert a particle momentum loss to its energy
  double xi_to_e( double xi, double e0 = Parameters::get()->beamEnergy() );
}

#endif
