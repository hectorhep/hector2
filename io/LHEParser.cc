#include "LHEParser.h"

namespace Hector
{
  namespace Parser
  {
    std::regex LHE::rgx_hdr_beg_( "^\\s*<init>\\s*$" ), LHE::rgx_hdr_end_( "^\\s*</init>\\s*$" );
    //std::regex LHE::rgx_hdr_( "<init>\\n\\s*([0-9+-]*)\\s+([0-9+-]*)\\s+([0-9+-.]+)\\s+([0-9+-.]+)\\s+([0-9+-]*)\\s+([0-9+-]*)\\s+([0-9+-]*)\\s+([0-9+-]*)\\s+([0-9+-]*)\\s+([0-9+-]*)\\s+([Ee0-9+-.]+)\\s+([Ee0-9+-.]*)\\s+([Ee0-9+-.]*)\\s([0-9+-]*)\\s*\\n<\\/init>" );
    std::regex LHE::rgx_hdr_line1_( "^\\s*([0-9+-]*)\\s+([0-9+-]*)\\s+([eE0-9+-.]+)\\s+([eE0-9+-.]+)\\s+([0-9+-]*)\\s+([0-9+-]*)\\s+([0-9+-]*)\\s+([0-9+-]*)\\s+([0-9+-]*)\\s+([0-9+-]*)\\s*$" ),
               LHE::rgx_hdr_line2_( "^\\s*([Ee0-9+-.]+)\\s+([Ee0-9+-.]*)\\s+([Ee0-9+-.]*)\\s([0-9+-]*)\\s*$" );
    std::regex LHE::rgx_evt_hdr_( "\\s*([0-9+-]+)\\s+([0-9+-]+)\\s+([eE0-9+-.]{13})\\s*([eE0-9+-.]{13})\\s*([eE0-9+-.]{13})\\s*([eE0-9+-.]{13})\\s*" );
    std::regex LHE::rgx_evt_prt_( "\\s*([0-9+-]+)\\s+([0-9+-]+)\\s+([0-9+-]+)\\s+([0-9+-]+)\\s+([0-9+-]+)\\s+([0-9+-]+)\\s+([eE0-9+-.]{16,17})\\s+([eE0-9+-.]{16,17})\\s+([eE0-9+-.]{16,17})\\s+([eE0-9+-.]{16,17})\\s+([eE0-9+-.]{16,17})\\s+([0-9+-.]+)\\s+([0-9+-.]+)" );

    LHE::LHE( const char* filename ) :
      has_header_( false )
    {
      in_file_ = std::ifstream( filename );
      parseHeader();
    }

    LHE::~LHE()
    {
      if ( in_file_.is_open() ) in_file_.close();
    }

    void
    LHE::printInfo() const
    {
      std::ostringstream os;
      os << "LHEF file successfully parsed. General info:"
         << "\n\t Beams: " << beam1_energy_ << " GeV (" << beam1_pdgid_ << ") on " << beam2_energy_ << " GeV (" << beam2_pdgid_ << ")"
         << "\n\t Process cross section: " << cross_section_ << " +- " << err_cross_section_ << " pb"
         << "\n\t QCD scale: " << qcd_scale_ << " GeV^-2";
      PrintInfo( os.str() );
    }

    void
    LHE::parseHeader()
    {
      std::string line;
      int i=0;
      bool in_header = false;
      while ( !in_file_.eof() && i<100 ) {
        std::getline( in_file_, line );
        if ( !in_header and std::regex_match( line, rgx_hdr_beg_ ) ) { in_header = true; continue; }
        else if ( in_header ) {
          if ( std::regex_match( line, rgx_hdr_end_ ) ) { return; }
          std::smatch match;
          std::cout << "---> " << line << std::endl;
          if ( std::regex_search( line, match, rgx_hdr_line1_ ) ) {
            beam1_pdgid_ = atoi( match.str( 1 ).c_str() );
            beam2_pdgid_ = atoi( match.str( 2 ).c_str() );
            beam1_energy_ = atof( match.str( 3 ).c_str() );
            beam2_energy_ = atof( match.str( 4 ).c_str() );
          }
          if ( std::regex_search( line, match, rgx_hdr_line2_ ) ) {
            cross_section_ = atof( match.str( 1 ).c_str() );
            err_cross_section_ = atof( match.str( 2 ).c_str() );
            qcd_scale_ = atof( match.str( 3 ).c_str() );
          }
        }

      }
    }

    Particles
    LHE::nextEvent()
    {
      Particles out;
      return out;
    }
  }
}
