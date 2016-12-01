#ifndef MADXParser_h
#define MADXParser_h

#include "beamline/Beamline.h"

#include "elements/ElementDict.h"

#include "elements/Quadrupole.h"
#include "elements/Dipole.h"
#include "elements/RectangularCollimator.h"
#include "elements/Kicker.h"
#include "elements/Marker.h"

#include "elements/EllipticAperture.h"
#include "elements/CircularAperture.h"
#include "elements/RectangularAperture.h"
#include "elements/RectEllipticAperture.h"

#include "core/ParametersMap.h"
#include "core/UnorderedParametersMap.h"

#include <fstream>
#include <regex>
#include <map>
#include <string>

using std::ostream;

namespace Hector
{
  namespace Parser
  {
    class MADX
    {
      public:
        MADX( const char*, const char*, int, float max_s=-1. );
        ~MADX();

        Beamline* beamline() const { return beamline_; }

      public:
        enum ValueType { Unknown = -1, String, Float, Integer };
        friend std::ostream& operator<<( std::ostream&, const ValueType& );

      private:
        void parseHeader();
        void parseElementsFields();
        void parseElements();

        ParametersMap<std::string> header_str_;
        ParametersMap<float> header_float_;

        UnorderedParametersMap<ValueType> elements_fields_;

        std::ifstream in_file_;
        std::streampos in_file_lastline_;

        std::string ip_name_;
        int dir_;
        Beamline* beamline_;

        static std::regex rgx_str_;

    };
  }
}

#endif
