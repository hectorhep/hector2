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

#ifndef Hector_ExceptionType_h
#define Hector_ExceptionType_h

namespace hector {
  /// Enumeration of exception severities
  enum struct ExceptionType {
    undefined = -1,  ///< Invalid exception type
    debug,           ///< Debugging information for developers
    info,            ///< Simple log to interact with the end user
    warning,         ///< Simple warning (proceeds with the execution)
    fatal            ///< Fatal error (exits the program)
  };
}  // namespace hector

#endif
