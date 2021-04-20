#ifndef Hector_IO_TwissHandler_h
#define Hector_IO_TwissHandler_h

#include "Hector/Utils/OrderedParametersMap.h"
#include "Hector/Utils/UnorderedParametersMap.h"

#include "Hector/Elements/ElementBase.h"
#include "Hector/Apertures/ApertureType.h"

#include <fstream>
#include <regex>
#include <string>
#include <memory>

using std::ostream;

namespace hector {
  class Beamline;
  namespace io {
    /// Parsing tool for MAD-X output Twiss files
    /// \note A list of variables stored in Twiss files can be retrieved from http://mad.web.cern.ch/mad/madx.old/Introduction/tables.html
    class Twiss {
    public:
      /// Class constructor
      /// \param[in] filename Path to the MAD-X Twiss file to parse
      /// \param[in] ip_name Name of the interaction point
      /// \param[in] min_s Minimal s-coordinate from which the Twiss file must be parsed
      /// \param[in] max_s Maximal s-coordinate at which the Twiss file must be parsed
      Twiss(std::string filename, std::string ip_name, float max_s = -1., float min_s = 0.);
      /// Copy constructor (without the beamline)
      Twiss(const Twiss&);
      /// Copy constructor
      Twiss(Twiss&);
      ~Twiss() {}

      /// Retrieve the sequenced beamline parsed from the MAD-X Twiss file
      Beamline* beamline() const;
      /// Retrieve the raw beamline parsed from the MAD-X Twiss file
      Beamline* rawBeamline() const { return raw_beamline_.get(); }

      /// Get a Hector element type from a Twiss element name string
      static element::Type findElementTypeByName(std::string name);
      /// Get a Hector element type from a Twiss element keyword string
      static element::Type findElementTypeByKeyword(std::string keyword);
      /// Get a Hector element aperture type from a Twiss element apertype string
      static aperture::Type findApertureTypeByApertype(std::string apertype);

      /// Print all useful information parsed from the MAD-X Twiss file
      void printInfo() const;
      /// List of all string variables parsed from the Twiss file
      std::map<std::string, std::string> headerStrings() const;
      /// List of all floating-point variables parsed from the Twiss file
      std::map<std::string, float> headerFloats() const;

    private:
      /// A collection of values to be propagated through this parser
      typedef std::vector<std::string> ValuesCollection;
      /// Type of content stored in the parameters map
      enum ValueType : short { Unknown = -1, String, Float, Integer };
      /// Human-readable printout of a value type
      friend std::ostream& operator<<(std::ostream&, const ValueType&);

      void parseHeader();
      void parseElementsFields();
      void parseElements();
      void findInteractionPoint();
      std::shared_ptr<element::ElementBase> parseElement(const ValuesCollection&);

      pmap::Ordered<std::string> header_str_;
      pmap::Ordered<float> header_float_;

      pmap::Unordered<ValueType> elements_fields_;

      std::ifstream in_file_;
      std::streampos in_file_lastline_;

      std::unique_ptr<Beamline> beamline_;
      std::unique_ptr<Beamline> raw_beamline_;
      std::shared_ptr<element::ElementBase> interaction_point_;

      std::string ip_name_;
      float min_s_;

      static std::regex rgx_typ_, rgx_hdr_, rgx_elm_hdr_;
      static std::regex rgx_drift_name_, rgx_ip_name_, rgx_monitor_name_;
      static std::regex rgx_quadrup_name_;
      static std::regex rgx_sect_dipole_name_, rgx_rect_dipole_name_;
      static std::regex rgx_rect_coll_name_;
    };
  }  // namespace io
}  // namespace hector

#endif
