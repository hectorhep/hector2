#ifndef Hector_IO_HBLFileHandler_h
#define Hector_IO_HBLFileHandler_h

#include <string>
#include <memory>

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
