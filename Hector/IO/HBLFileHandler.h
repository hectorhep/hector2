#ifndef Hector_IO_HBLFileHandler_h
#define Hector_IO_HBLFileHandler_h

#include <memory>

namespace Hector
{
  class Beamline;
  /// Collection of input/output utilitaries
  namespace IO
  {
    /// An HBL (Hector BeamLine) files handler
    class HBL
    {
      public:
        /// Parse an external HBL file
        HBL( const char* );
        /// Parse an external HBL file
        HBL( std::string filename ) : HBL( filename.c_str() ) {}
        HBL( const HBL& ) {}
        HBL( HBL& );
        ~HBL() {}

        /// Parse an external HBL file
        void parse( const char* );
        /// Write a beamline to an external HBL file
        static void write( const Beamline*, const char* filename );
        /// Retrieve the beamline parsed from an external HBL file
        Beamline* beamline() const { return beamline_.get(); };

      private:
        std::unique_ptr<Beamline> beamline_;
        static constexpr unsigned long long magic_number = 0x464c4248;
        static constexpr unsigned short version = 100;
    };
  }
}

#endif

