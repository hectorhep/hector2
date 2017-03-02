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

#include "core/OrderedParametersMap.h"
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
    /// Parsing tool for MAD-X output stp files
    class MADX
    {
      public:
        /// A collection of values to be propagated through this parser
        typedef std::vector<std::string> ValuesCollection;

      public:
        /// Class constructor
        MADX( const char*, const char*, int, float max_s=-1. );
        ~MADX();

        /// Retrieve the beamline parsed from the MAD-X output file
        Beamline* beamline() const { return beamline_; }

      public:
        /// Type of content stored in the parameters map
        enum ValueType { Unknown = -1, String, Float, Integer };
        /// Human-readable printout of a value type
        friend std::ostream& operator<<( std::ostream&, const ValueType& );
        /// Print all useful information parsed from the MAD-X output file
        void printInfo() const;

      private:
        void parseHeader();
        void parseElementsFields();
        void parseElements();
        void parseElement( const ValuesCollection& );

        ParametersMap::Ordered<std::string> header_str_;
        ParametersMap::Ordered<float> header_float_;

        ParametersMap::Unordered<ValueType> elements_fields_;

        std::ifstream in_file_;
        std::streampos in_file_lastline_;

        std::string ip_name_;
        int dir_;
        Beamline* beamline_;
        float s_offset_;
        bool found_interaction_point_;
        // quantities needed whenever direction == 1 (FIXME)
        CLHEP::Hep2Vector previous_relpos_, previous_disp_, previous_beta_;

        static std::regex rgx_typ_, rgx_hdr_, rgx_elm_hdr_;

    };
  }
}

#endif
