#include "LHEParser.h"

namespace Hector
{
  namespace Parser
  {
    LHE::LHE( const char* filename ) :
      reader_( filename )
    {}

    LHE::~LHE()
    {}

    void
    LHE::printInfo() const
    {
      std::ostringstream os;
      os << "LHEF file successfully parsed. General info:"
         << "\n\t Beams: " << beam1Energy() << " GeV (" << beam1PDGId() << ") on " << beam2Energy() << " GeV (" << beam2PDGid() << ")"
         << "\n\t Process cross section: " << crossSection() << " +- " << crossSectionError() << " pb";
      PrintInfo( os.str() );
    }

    bool
    LHE::nextEvent( Particles& parts )
    {
      parts.clear();
      bool status = reader_.readEvent();
      if ( !status ) return false;
      for ( int i=0; i<reader_.hepeup.NUP; i++ ) {
        CLHEP::HepLorentzVector mom( reader_.hepeup.PUP[i][0], reader_.hepeup.PUP[i][1], reader_.hepeup.PUP[i][2], reader_.hepeup.PUP[i][3] );
        Particle part( mom );
        part.setPDGid( reader_.hepeup.IDUP[i] );
        parts.push_back( part );
      }
      return true;
    }
  }
}
