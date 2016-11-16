#ifndef MadXparser_h
#define MadXparser_h

#include "beamline/Beamline.h"

#include "elements/ElementDict.h"
#include "elements/HorizontalQuadrupole.h"
#include "elements/VerticalQuadrupole.h"
#include "elements/RectangularDipole.h"
#include "elements/SectorDipole.h"

#include "core/ParametersMap.h"
#include "core/UnorderedParametersMap.h"

#include <fstream>
#include <regex>
#include <map>
#include <string>

using std::ostream;

namespace Parser
{
  class MadX
  {
    public:
      MadX( const char*, int );
      ~MadX();

      Beamline beamline() const { return beamline_; }

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

      int dir_;
      Beamline beamline_;

      static std::regex rgx_str_;

  };
}

#endif
