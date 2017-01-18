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
        const short status = reader_.hepeup.ISTUP[i],
                    pdg_id = reader_.hepeup.IDUP[i];

        const CLHEP::HepLorentzVector mom( reader_.hepeup.PUP[i][0], reader_.hepeup.PUP[i][1], reader_.hepeup.PUP[i][2], reader_.hepeup.PUP[i][3] );

        if ( status==-1 and pdg_id!=2212 ) { // only accept protons as primary beam particles (FIXME: use mass to filter?)
          const short sign = ( mom.pz()>0. ) ? +1 : -1;
          const double pz = sqrt( pow( Constants::beam_energy, 2 )-pow( Constants::beam_particles_mass, 2 ) );
          CLHEP::HepLorentzVector prim( 0., 0., sign*pz, Constants::beam_energy );
          Particle part( prim-mom );
          part.setPDGid( 2212 );
          part.firstStateVector().setM( Constants::beam_particles_mass );
          parts.push_back( part );
          continue;
        }

        Particle part( mom );
        part.setPDGid( pdg_id );
        if ( status==2 ) { // keep final-state particles
          parts.push_back( part );
        }
      }
      return true;
    }
  }
}
