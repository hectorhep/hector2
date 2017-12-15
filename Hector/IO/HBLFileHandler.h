#ifndef Hector_IO_HBLFileHandler_h
#define Hector_IO_HBLFileHandler_h

#include <memory>

namespace Hector
{
  class Beamline;
  namespace IO
  {
    class HBL
    {
      public:
        HBL( const char* );
        HBL( std::string filename ) : HBL( filename.c_str() ) {}
        ~HBL() {}

        void parse( const char* );
        static void write( const Beamline*, const char* filename );
        static void write( const Beamline* bl, std::string file ) { write( bl, file.c_str() ); }
        Beamline* beamline() const { return bl_.get(); };

      private:
        std::unique_ptr<Beamline> bl_;
        static constexpr unsigned long long magic_number = 0x464c4248;
        static constexpr unsigned short version = 1;
    };
  }
}

#endif

