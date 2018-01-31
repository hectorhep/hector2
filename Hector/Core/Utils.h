#ifndef Hector_Core_Utils_h
#define Hector_Core_Utils_h

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
}

#endif
