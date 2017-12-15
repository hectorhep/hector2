#include "Hector/IO/LHEHandler.h"

#ifndef GOOD_HEPMC
#pragma message "HepMC version 2 was found on your system! Version 3 is required for LHEF parsing!"
#else

#include "Hector/Core/Exception.h"

namespace Hector
{
  namespace IO
  {
    LHE::LHE( const char* filename ) :
      reader_( new LHEF::Reader( filename ) )
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
      bool status = reader_->readEvent();
      if ( !status ) return false;

      for ( int i = 0; i < reader_->hepeup.NUP; ++i ) {
        const short status = reader_->hepeup.ISTUP[i],
                    pdg_id = reader_->hepeup.IDUP[i];

        const LorentzVector mom( reader_->hepeup.PUP[i][0], reader_->hepeup.PUP[i][1], reader_->hepeup.PUP[i][2], reader_->hepeup.PUP[i][3] );

        // only accept protons as primary beam particles
        // (FIXME: use mass to filter?)
        if ( status == -1 && pdg_id != 2212 ) {
          const short sign = ( mom.pz()>0. ) ? +1 : -1;
          const double pz = sqrt( pow( Parameters::get()->beamEnergy(), 2 )-pow( Parameters::get()->beamParticlesMass(), 2 ) );
          LorentzVector prim( 0., 0., sign*pz, Parameters::get()->beamEnergy() );
          Particle part( prim-mom );
          part.setPDGid( 2212 );
          part.firstStateVector().setM( Parameters::get()->beamParticlesMass() );
          parts.push_back( part );
          continue;
        }

        Particle part( mom );
        part.setPDGid( pdg_id );
        if ( status == 2 ) { // keep final-state particles
          parts.push_back( part );
        }
      }
      return true;
    }

    float
    LHE::crossSection() const
    {
      return *( reader_->heprup.XSECUP.begin() );
    }

    float
    LHE::crossSectionError() const
    {
      return *( reader_->heprup.XERRUP.begin() );
    }

    int
    LHE::beam1PDGId() const
    {
      return reader_->heprup.IDBMUP.first;
    }

    int
    LHE::beam2PDGid() const
    {
      return reader_->heprup.IDBMUP.second;
    }

    float
    LHE::beam1Energy() const
    {
      return reader_->heprup.EBMUP.first;
    }

    float
    LHE::beam2Energy() const
    {
      return reader_->heprup.EBMUP.second;
    }
  }
}

#endif
