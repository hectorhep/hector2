#include "Hector/IO/MADXParser.h"
#include "Hector/Propagator/Propagator.h"
//#include "Hector/Propagator/BeamProducer.h"

#include "utils.h"
#include "Canvas.h"

#include <CLHEP/Units/SystemOfUnits.h>

#include "TH1.h"

int
main( int argc, char* argv[] )
{
  if ( argc<2 ) {
    std::cout << "Usage: " << argv[0] << " [MAD-X output file] <IP name>" << std::endl;
    return -1;
  }
  const char* ip = ( argc>2 ) ? argv[2] : "IP5";

  Hector::Parser::MADX parser( argv[1], ip, +1, 250. );
  //parser.printInfo();
  parser.beamline()->dump();
  //parser.beamline()->offsetElementsAfter( 120., CLHEP::Hep2Vector( -0.097, 0. ) );
  //Hector::Parameters::get()->useRelativeEnergy() = true;
  //Hector::Parameters::get()->enableKickers() = true;
  //Hector::Parameters::get()->enableDipoles() = false; //FIXME
  Hector::Parameters::get()->computeApertureAcceptance() = false; //FIXME

  Hector::Propagator prop( parser.beamline() );

  std::map<const char*,const char*> rp_names;
  rp_names.insert( std::make_pair( "56-210-nr-hr", "XRPH.C6R5.B1" ) );
  rp_names.insert( std::make_pair( "56-210-fr-hr", "XRPH.D6R5.B1" ) );
  rp_names.insert( std::make_pair( "56-220-nr-hr", "XRPH.A6R5.B1" ) );
  rp_names.insert( std::make_pair( "56-220-fr-hr", "XRPH.B6R5.B1" ) );

  std::cout << "list of horizontal roman pots: " << std::endl;
  const Hector::Elements h_rp = parser.romanPots( Hector::Parser::MADX::horizontalPots );
  for ( unsigned short i=0; i<h_rp.size(); i++ ) {
    std::cout << ">> " << h_rp[i]->name() << " at s = " << h_rp[i]->s() << " m" << std::endl;
  }

  const Hector::Element::ElementBase* rp = parser.beamline()->getElement( rp_names["56-210-fr-hr"] );
  if ( !rp ) {
    std::cerr << "Failed to retrieve Roman pot " << rp_names["56-210-fr-hr"] << " in beamline!" << std::endl;
    return -1;
  }
  std::cout << "---> RP " << rp_names["56-210-fr-hr"] << " at " << rp->s() << " m" << std::endl;

  TGraph gr_x_vs_xi, gr_y_vs_xi;

  const float cross_angle = -191.546e-6, // in rad
              y0_pos = 200.e-6; // in m

  CLHEP::Hep2Vector pos_rp0;
  Hector::Particle p = Hector::Particle::fromMassCharge( Hector::Parameters::get()->beamParticlesMass(), +1 );
  p.firstStateVector().setXi( 0. );
  p.firstStateVector().setPosition( 0., y0_pos );
  p.firstStateVector().setAngles( cross_angle, 0. );
  std::cout << p.firstStateVector() << std::endl;
  try {
    prop.propagate( p, rp->s() );
    pos_rp0 = p.stateVectorAt( rp->s() ).position();
    std::cout << "pos_0: " << pos_rp0 << std::endl;
  } catch ( Hector::Exception& e ) {}


  const float xi_values[] = { 0., 0.05, 0.1, 0.15 };
  const unsigned short num_values = sizeof( xi_values )/sizeof( xi_values[0] );
  for ( unsigned short i=0; i<num_values; i++ ) {
    Hector::Particle p = Hector::Particle::fromMassCharge( Hector::Parameters::get()->beamParticlesMass(), +1 );
    p.firstStateVector().setPosition( 0., y0_pos );
    p.firstStateVector().setXi( xi_values[i] );
    std::cout << p.firstStateVector().xi() << "\t" << p.firstStateVector().energy() << std::endl;
    //p.firstStateVector().setAngles( Hector::Parameters::get()->crossingAngleX()/2., Hector::Parameters::get()->crossingAngleY()/2. );
    p.firstStateVector().setAngles( cross_angle, 0. );
    try {
      prop.propagate( p, rp->s() );
      CLHEP::Hep2Vector pos_rp = p.stateVectorAt( rp->s() ).position();
      gr_x_vs_xi.SetPoint( gr_x_vs_xi.GetN(), xi_values[i], pos_rp.x()*1.e3 );
      gr_y_vs_xi.SetPoint( gr_y_vs_xi.GetN(), xi_values[i], pos_rp.y()*1.e3 );
    } catch ( Hector::Exception& e ) { e.dump(); }
  }

  {
    Hector::Canvas c( "x_vs_xi", "", true );
    c.cd( 1 );
    gr_x_vs_xi.Draw( "alp" );
    gr_x_vs_xi.SetMarkerStyle( 24 );
    gr_x_vs_xi.SetTitle( "#xi\\x (mm)" );
    c.Prettify( gr_x_vs_xi.GetHistogram() );
    ( (TPad*)c.GetPad( 1 ) )->SetGrid( 1, 1 );
    c.cd( 2 );
    gr_y_vs_xi.Draw( "alp" );
    gr_y_vs_xi.SetMarkerStyle( 24 );
    gr_y_vs_xi.SetTitle( "#xi\\y (mm)" );
    c.Prettify( gr_y_vs_xi.GetHistogram() );
    ( (TPad*)c.GetPad( 2 ) )->SetGrid( 1, 1 );
    c.Save( "pdf" );
  }

  return 0;
}
