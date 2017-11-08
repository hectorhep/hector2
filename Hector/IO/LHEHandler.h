#ifndef Hector_IO_LHEParser_h
#define Hector_IO_LHEParser_h

#include "Hector/Propagator/Particle.h"

#include "LHEF.h"

namespace Hector
{
  namespace IO
  {
    /// Parsing tool for LHEF files to use as beams of primary particles
    class LHE
    {
      public:
        /// Parse a LHEF file to build a particles vector for each event
        /// \param[in] filename Path to the .lhe file to parse
        LHE( const char* filename );
        ~LHE();

        /// Display general information retrieved from the LHEF file
        void printInfo() const;

        /// Retrieve the next event in the LHEF bank
        /// \return True, if the event retrieved is valid, false otherwise
        /// \param[out] parts Vector of final state particles contained in the event
        bool nextEvent( Particles& parts );

        /// Process cross section (in pb)
        float crossSection() const { return *( reader_.heprup.XSECUP.begin() ); }
        /// Error on the process cross section (in pb)
        float crossSectionError() const { return *( reader_.heprup.XERRUP.begin() ); }

        /// PDG id of the positive z beam
        int beam1PDGId() const { return reader_.heprup.IDBMUP.first; }
        /// PDG id of the negative z beam
        int beam2PDGid() const { return reader_.heprup.IDBMUP.second; }
        /// Energy of the positive z beam (in GeV)
        float beam1Energy() const { return reader_.heprup.EBMUP.first; }
        /// Energy of the negative z beam (in GeV)
        float beam2Energy() const { return reader_.heprup.EBMUP.second; }

      private:
        LHEF::Reader reader_;
    };
  }
}

#endif
