#ifndef Hector_IO_HBLFileHandler_h
#define Hector_IO_HBLFileHandler_h

#include "Hector/Elements/ElementType.h"
#include "Hector/Elements/ApertureType.h"
#include "Hector/Beamline/Beamline.h"

#include <memory>

namespace Hector
{
  namespace IO
  {
    class HBL
    {
      public:
        HBL( const char* );
        ~HBL() {}

        void parse( const char* );
        static void write( const Beamline*, const char* filename );
        Beamline* beamline() const { return bl_.get(); };

      private:
        std::unique_ptr<Beamline> bl_;
        static constexpr unsigned long long magic_number = 0x464c4248;
        static constexpr unsigned short version = 1;
    };
  }
}

#endif

