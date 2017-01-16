#ifndef IO_LHEParser_h
#define IO_LHEParser_h

#include "propagator/Particle.h"

#include <fstream>
#include <regex>

namespace Hector
{
  namespace Parser
  {
    class LHE
    {
      public:
        LHE( const char* );
        ~LHE();

        void printInfo() const;

        Particles nextEvent();

        float crossSection() const { return cross_section_; }
        float crossSectionError() const { return err_cross_section_; }

      private:
        void parseHeader();

        static std::regex rgx_hdr_beg_, rgx_hdr_end_, rgx_hdr_line1_, rgx_hdr_line2_, rgx_evt_hdr_, rgx_evt_prt_;

        std::ifstream in_file_;
        bool has_header_;
        float cross_section_, err_cross_section_;
        int beam1_pdgid_, beam2_pdgid_;
        float beam1_energy_, beam2_energy_;
        float qcd_scale_;
    };
  }
}

#endif
