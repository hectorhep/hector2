#ifndef Hector_Utils_String_h
#define Hector_Utils_String_h

#include "Hector/Parameters.h"

#include <string>

namespace hector {
  /// Format a string using a printf style format descriptor.
  std::string format(const std::string fmt, ...);
  /// Remove leading and trailing whitespaces to a string
  std::string trim(const std::string& str);
  /// Get the lowercase version of a string
  std::string lowercase(const std::string& str);
}  // namespace hector

#endif
