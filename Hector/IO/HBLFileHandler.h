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

#ifndef Hector_IO_HBLFileHandler_h
#define Hector_IO_HBLFileHandler_h

#include <memory>
#include <string>

namespace hector {
  class Beamline;
  /// Collection of input/output utilitaries
  namespace io {
    /// An HBL (Hector BeamLine) files handler
    class HBL {
    public:
      /// Parse an external HBL file
      HBL(const std::string& filename);
      HBL(const HBL&) {}
      HBL(HBL&);
      ~HBL() {}

      /// Parse an external HBL file
      void parse(const std::string&);
      /// Write a beamline to an external HBL file
      static void write(const Beamline*, const std::string& filename);
      /// Retrieve the beamline parsed from an external HBL file
      Beamline* beamline() const { return beamline_.get(); };

    private:
      std::unique_ptr<Beamline> beamline_;
      static constexpr unsigned long long magic_number = 0x464c4248;
      static constexpr unsigned short version = 100;
    };
  }  // namespace io
}  // namespace hector

#endif
