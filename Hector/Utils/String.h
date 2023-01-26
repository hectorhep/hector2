/*
 *  Hector: a beamline propagation tool
 *  Copyright (C) 2016-2023  Laurent Forthomme
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
  std::string now(const std::string& fmt = "%Y-%d-%m %H:%M:%S");
}  // namespace hector

#endif
