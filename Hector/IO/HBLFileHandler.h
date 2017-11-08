#ifndef Hector_IO_HBLFile_h
#define Hector_IO_HBLFile_h

#include "Hector/Elements/ElementType.h"
#include "Hector/Elements/ApertureType.h"
#include "Hector/Beamline/Beamline.h"

#include <array>
#include <memory>

namespace Hector
{
  struct HBLHeader
  {
    unsigned long long magic;
    unsigned long version;
  };
  struct HBLElement
  {
    Element::Type elem_type;
    std::string elem_name;
    double mag_str;
    Aperture::Type aper_type;
    std::array<double,4> aper_params;
  };
  namespace Parser
  {
    class HBL
    {
      public:
        HBL( const char* );
        ~HBL() {}

        Beamline* beamline() const { return bl_.get(); };

      private:
        std::unique_ptr<Beamline> bl_;
        static constexpr unsigned long long magic_number = 0x464c4248;
    };
  }
}

#endif

