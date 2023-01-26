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

#ifndef Hector_Core_Timer_h
#define Hector_Core_Timer_h

#include <chrono>

namespace hector {
  /**
   * A generic timer to extract the processing time between two steps in this software's flow
   * \author Laurent Forthomme <laurent.forthomme@cern.ch>
   */
  class Timer {
  public:
    inline Timer() { reset(); }
    /**
     * Get the time elapsed since the last @a reset call (or class construction)
     * \return Elapsed time (since the last reset), in seconds
     */
    inline double elapsed() {
      auto end = std::chrono::high_resolution_clock::now();
      return std::chrono::duration<double>(end - beg_).count();
    }
    /// Reset the clock counter
    inline void reset() { beg_ = std::chrono::high_resolution_clock::now(); }

  private:
    /// Timestamp marking the beginning of the counter
    std::chrono::time_point<std::chrono::high_resolution_clock> beg_;
  };
}  // namespace hector

#endif
