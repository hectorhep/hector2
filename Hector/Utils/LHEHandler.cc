#include "Hector/Utils/LHEHandler.h"

#ifndef GOOD_HEPMC
#  ifndef PYTHIA8
#    pragma message "No HepMC v3+ nor Pythia8 release was found on your system! Disabling LHEF parsing!"
#  endif
#endif

#include "Hector/Core/Exception.h"

#include <sstream>

namespace Hector
{
  namespace IO
  {
    LHE::LHE( const char* filename )
#ifdef GOOD_HEPMC
      : reader_( new LHEF::Reader( filename ) )
    {}
#else
#  ifdef PYTHIA8
      : pythia_( new Pythia8::Pythia )
    {
      pythia_->settings.mode( "Next:numberCount", 0 ); // disabling unnecessary output
      pythia_->settings.mode( "Beams:frameType", 4 ); // LHEF input
      pythia_->settings.word( "Beams:LHEF", filename );
      pythia_->init();
    }
#  else
    {}
#  endif
#endif

    LHE::~LHE()
    {}

    void
    LHE::printInfo() const
    {
      PrintInfo
       << "LHEF file successfully parsed. General info:"
       << "\n\t Beams: " << beam1Energy() << " GeV (" << beam1PDGId() << ") on " << beam2Energy() << " GeV (" << beam2PDGid() << ")"
       << "\n\t Process cross section: " << crossSection() << " +- " << crossSectionError() << " pb.";
    }

    bool
    LHE::nextEvent( Particles& parts )
    {
      parts.clear();
#ifdef GOOD_HEPMC
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
#else
#  ifdef PYTHIA8
      while ( true ) {
        if ( !pythia_->next() )
          continue;
        for ( int i = 0; i < pythia_->event.size(); ++i ) {
          const Pythia8::Particle& pyp = pythia_->event[i];
          if ( pyp.status() > 0 ) {
            const LorentzVector mom( pyp.px(), pyp.py(), pyp.pz(), pyp.e() );
            Particle part( mom );
            part.setPDGid( pyp.id() );
            parts.emplace_back( part );
          }
        }
        return true;
      }
#  endif
#endif
      return true;
    }

    float
    LHE::crossSection() const
    {
#ifdef GOOD_HEPMC
      return *( reader_->heprup.XSECUP.begin() );
#else
      return -1.;
#endif
    }

    float
    LHE::crossSectionError() const
    {
#ifdef GOOD_HEPMC
      return *( reader_->heprup.XERRUP.begin() );
#else
      return -1.;
#endif
    }

    int
    LHE::beam1PDGId() const
    {
#ifdef GOOD_HEPMC
      return reader_->heprup.IDBMUP.first;
#else
      return 0;
#endif
    }

    int
    LHE::beam2PDGid() const
    {
#ifdef GOOD_HEPMC
      return reader_->heprup.IDBMUP.second;
#else
      return 0;
#endif
    }

    float
    LHE::beam1Energy() const
    {
#ifdef GOOD_HEPMC
      return reader_->heprup.EBMUP.first;
#else
      return -1.;
#endif
    }

    float
    LHE::beam2Energy() const
    {
#ifdef GOOD_HEPMC
      return reader_->heprup.EBMUP.second;
#else
      return -1.;
#endif
    }
  }
}

