#include "Hector/IO/Pythia8Generator.h"
#include "Hector/Core/Parameters.h"
#include "Hector/Core/Utils.h"
#include "Hector/Core/Exception.h"

#include <Pythia8/Event.h>

namespace Hector
{
  Pythia8Generator::Pythia8Generator( const std::vector<std::string>& params ) :
    pythia_( std::make_unique<Pythia8::Pythia>() )
  {
    // specify the incoming state
    pythia_->settings.mode( "Beams:idA", 2212 );
    pythia_->settings.mode( "Beams:idB", 2212 );
    pythia_->settings.parm( "Beams:eCM", 2.*Parameters::get()->beamEnergy() ); //FIXME consider the asymmetric energies too!
    // parameterise the generator
    for ( const auto param : params ) {
      if ( !pythia_->readString( param.c_str() ) ) {
        throw Exception( __PRETTY_FUNCTION__, Form( "Failed to parse the command:\n\t  \"%s\"", param.c_str() ), JustWarning );
      }
    }
    // initialise the core
    if ( !pythia_->init() ) {
      throw Exception( __PRETTY_FUNCTION__, "Failed to initialise the Pythia8 core", JustWarning );
    }
  }

  Pythia8Generator::Pythia8Generator( const char* xml_input ) :
    pythia_( std::make_unique<Pythia8::Pythia>( xml_input ) )
  {
    // initialise the core
    if ( !pythia_->init() ) {
      throw Exception( __PRETTY_FUNCTION__, "Failed to initialise the Pythia8 core", JustWarning );
    }
  }

  Pythia8Generator::~Pythia8Generator()
  {
    // dump some (useful?) generation statistics
    pythia_->stat();
  }

  Particles
  Pythia8Generator::generate()
  {
    pythia_->next();
    const Pythia8::Event evt = pythia_->event;

    Particles pout;
    for ( unsigned short i=0; i<evt.size(); i++ ) {
      const Pythia8::Particle part = evt[i];
      if ( part.id() == 9902210 ) { // diffractive proton
        pout.emplace_back( CLHEP::HepLorentzVector( part.px(), part.py(), part.pz(), part.e() ), (int)part.charge() );
      }
    }
    return pout;
  }

  Particle
  Pythia8Generator::diffractiveProton()
  {
    pythia_->next();

    const Pythia8::Event evt = pythia_->event;
    for ( unsigned short i=0; i<evt.size(); i++ ) {
      const Pythia8::Particle part = evt[i];
      if ( part.id() == 9902210 ) { // diffractive proton
        return Particle( CLHEP::HepLorentzVector( part.px(), part.py(), part.pz(), part.e() ), (int)part.charge() );
      }
    }
    throw Exception( __PRETTY_FUNCTION__, "Failed to generate the diffractive proton!", JustWarning );
  }

  void
  Pythia8Generator::generate( Particle& in_part )
  {
    pythia_->next();
    in_part.clear();

    const Pythia8::Event evt = pythia_->event;
    for ( unsigned short i=0; i<evt.size(); i++ ) {
      const Pythia8::Particle part = evt[i];
      if ( part.id()==9902210 ) { // diffractive proton
        in_part.firstStateVector().addMomentum( CLHEP::HepLorentzVector( part.px(), part.py(), part.pz(), part.e() ) );
        return;
      }
    }
    throw Exception( __PRETTY_FUNCTION__, "Failed to generate the modified kinematics for the particle template!", JustWarning );
  }
}
