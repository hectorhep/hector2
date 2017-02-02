#include "io/MADXParser.h"
#include "propagator/Propagator.h"

int main( int argc, char* argv[] )
{
  Hector::Parser::MADX parser( "data/twiss/twiss_coll0p4m_ir5b1_6p5tev.tfs", "IP5", +1, 250. );

  std::cout << "beamline matrix: " << parser.beamline()->matrix( 100., Hector::Constants::beam_particles_mass, +1 ) << std::endl;

  Hector::Propagator prop( parser.beamline() );
  parser.beamline()->dump();

  CLHEP::HepLorentzVector mom0( 0., 0., 0., Hector::Constants::beam_particles_mass );
  for ( unsigned int i=0; i<10; i++ ) {
    const float pz = Hector::Constants::beam_energy - ( i+1 )*10.;
    mom0.setPz( pz );
    mom0.setE( sqrt( pz*pz+std::pow( Hector::Constants::beam_particles_mass, 2 ) ) );
    Hector::Particle p( mom0 );
    p.setCharge( +1 );
    try {
      prop.propagate( p, 220. );
    } catch ( Hector::Exception& e ) { e.dump(); }
    //p.dump();
  }

  return 0;
}
