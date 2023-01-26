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

#ifndef Hector_ParticleStoppedException_h
#define Hector_ParticleStoppedException_h

#include "Hector/Elements/ElementBase.h"
#include "Hector/Elements/ElementBaseFwd.h"
#include "Hector/Exception.h"

namespace hector {
  /// Specific exception for particles stopped in the course of a beamline propagation
  class ParticleStoppedException : public Exception {
  public:
    /// Exception constructor
    /// \param[in] from Method raising the exception
    /// \param[in] elem Beamline element stopping the particle
    /// \param[in] type Exception type
    /// \param[in] additional_info Any other useful information to help the debugging
    ParticleStoppedException(const std::string& from,
                             ExceptionType type = ExceptionType::undefined,
                             const element::ElementPtr& elem = nullptr)
        : Exception(from, type, 10000), elem_(elem) {
      message_ << "Particle stopped";
      if (elem)
        message_ << " at " << elem->name() << " (" << elem->type() << ")";
      message_ << ".\n";
    }
    /// Retrieve the beamline element that stopped the particle
    const element::ElementPtr& stoppingElement() const { return elem_; }

  private:
    /// Beamline element that stopped the particle
    const element::ElementPtr& elem_;
  };
}  // namespace hector

#endif
