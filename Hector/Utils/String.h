#ifndef Hector_Utils_String_h
#define Hector_Utils_String_h

#include <string>

namespace hector {
  /// Format a string using a printf style format descriptor.
  std::string format(const std::string fmt, ...);
  /// Remove leading and trailing whitespaces to a string
  std::string trim(const std::string& str);
  /// Get the lowercase version of a string
  std::string lowercase(const std::string& str);
  /// Enumeration of possible TTY colours
  enum class Colour {
    reset = -1,
    black = 30,
    red = 31,
    green = 32,
    yellow = 33,
    blue = 34,
    magenta = 35,
    cyan = 36,
    white = 37
  };
  /// TTY-type enumeration of text styles
  enum class Modifier { reset = -1, bold = 1, dimmed = 2, italic = 3, underline = 4, blink = 5, reverse = 7 };
  /// Set the TTY colour and font style for a string
  std::string colourise(const std::string& str, Colour col, Modifier mod);
  /// Current human-readable date and time
  std::string now();
}  // namespace hector

#endif
