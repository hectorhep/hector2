#ifndef Hector_Core_Utils_h
#define Hector_Core_Utils_h

#include <string>

namespace CLHEP
{
  class Hep2Vector;
}

namespace Hector
{
  /// Format a string using a printf style format descriptor.
  std::string Form( const std::string fmt, ... );
  /// Remove leading and trailing whitespaces to a string
  std::string trim( const std::string& str );
  /// Get the lowercase version of a string
  std::string lowercase( const std::string& str );

  namespace math
  {
    /// Compute the tangent of both the components of a 2-vector
    CLHEP::Hep2Vector tan2( const CLHEP::Hep2Vector& ang );
    /// Compute the arc-tangent of both the components of a 2-vector
    CLHEP::Hep2Vector atan2( const CLHEP::Hep2Vector& ang );
  }
}

#endif
