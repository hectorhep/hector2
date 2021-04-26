#include "Hector/IO/TwissHandler.h"
#include "Hector/Exception.h"
#include "Hector/ParticleStoppedException.h"
#include "Hector/Beamline.h"
#include "Hector/Propagator.h"
//#include "Hector/Utils/BeamProducer.h"
#include "Hector/Utils/ArgsParser.h"

#include "utils.h"
#include "Canvas.h"

#include <CLHEP/Units/SystemOfUnits.h>

#include "TH1.h"

using namespace std;

int main(int argc, char* argv[]) {
  string twiss_file, ip_name;
  double max_s;
  hector::ArgsParser(argc,
                     argv,
                     {{"twiss-file", "beamline Twiss file", &twiss_file, 'i'}},
                     {
                         {"ip-name", "name of the interaction point", "IP5", &ip_name, 'c'},
                         {"max-s", "maximal s-coordinate (m)", 250., &max_s},
                     });

  hector::io::Twiss parser(twiss_file, ip_name, max_s);
  parser.printInfo();
  //parser.beamline()->dump();
  //parser.beamline()->offsetElementsAfter( 120., CLHEP::Hep2Vector( -0.097, 0. ) );
  //hector::Parameters::get()->setUseRelativeEnergy( true );
  //hector::Parameters::get()->setEnableKickers( true );
  //hector::Parameters::get()->setEnableDipoles( false ); //FIXME
  hector::Parameters::get()->setComputeApertureAcceptance(false);  //FIXME

  hector::Propagator prop(parser.beamline());

  map<const char*, const char*> rp_names = {
      {"45-210-nr-hr", "XRPH.C6R5.B2"},
      {"45-210-fr-hr", "XRPH.D6R5.B2"},
      {"56-210-nr-hr", "XRPH.C6R5.B1"},
      {"56-210-fr-hr", "XRPH.D6R5.B1"},
      //{ "56-220-nr-hr", "XRPH.A6R5.B1" },
      //{ "56-220-fr-hr", "XRPH.B6R5.B1" }
  };

  H_INFO.log([&](auto& log) {
    log << "list of horizontal roman pots:";
    const auto& h_rp = parser.beamline()->find("XRPH\\.");
    for (const auto& rp : h_rp) {
      log << "\n>> " << rp->name() << " at s = " << rp->s() << " m.";
    }
  });

  const auto rp = parser.beamline()->get(rp_names["56-210-fr-hr"]);
  if (!rp)
    throw H_ERROR << "Failed to retrieve Roman pot " << rp_names["56-210-fr-hr"] << " in beamline!";
  H_INFO << "---> RP " << rp_names["56-210-fr-hr"] << " at " << rp->s() << " m.";

  TGraph gr_x_vs_xi, gr_y_vs_xi;

  const float cross_angle = 191.546e-6,  // in rad
      y0_pos = 200.e-6;                  // in m

  CLHEP::Hep2Vector pos_rp0;
  hector::Particle p = hector::Particle::fromMassCharge(hector::Parameters::get()->beamParticlesMass(), +1);
  p.firstStateVector().setXi(0.);
  p.firstStateVector().setPosition(0., y0_pos);
  p.firstStateVector().setAngles(cross_angle, 0.);
  cout << p.firstStateVector() << endl;
  try {
    prop.propagate(p, rp->s());
    pos_rp0 = p.stateVectorAt(rp->s()).position();
    cout << "pos_0: " << pos_rp0 << endl;
  } catch (hector::ParticleStoppedException& e) {
  } catch (hector::Exception& e) {
  }

  const vector<double> xi_values = {{0.01, 0.05, 0.1, 0.15}};
  for (const auto& xi : xi_values) {
    hector::Particle p = hector::Particle::fromMassCharge(hector::Parameters::get()->beamParticlesMass(), +1);
    p.firstStateVector().setPosition(0., y0_pos);
    p.firstStateVector().setAngles(cross_angle, 0.);
    p.firstStateVector().setXi(xi);
    cout << p.firstStateVector().xi() << "\t" << p.firstStateVector() << endl;
    try {
      prop.propagate(p, rp->s());
      CLHEP::Hep2Vector pos_rp = p.stateVectorAt(rp->s()).position();
      cout << pos_rp << endl;
      gr_x_vs_xi.SetPoint(gr_x_vs_xi.GetN(), xi, pos_rp.x() * 1.e3);
      gr_y_vs_xi.SetPoint(gr_y_vs_xi.GetN(), xi, pos_rp.y() * 1.e3);
    } catch (hector::Exception& e) {
      e.dump(std::cerr);
    }
  }

  {
    hector::Canvas c("x_vs_xi", "", true);
    c.cd(1);
    gr_x_vs_xi.Draw("alp");
    gr_x_vs_xi.SetMarkerStyle(24);
    gr_x_vs_xi.SetTitle("#xi@@x (mm)");
    c.Prettify(gr_x_vs_xi.GetHistogram());
    ((TPad*)c.GetPad(1))->SetGrid(1, 1);
    c.cd(2);
    gr_y_vs_xi.Draw("alp");
    gr_y_vs_xi.SetMarkerStyle(24);
    gr_y_vs_xi.SetTitle("#xi@@y (mm)");
    c.Prettify(gr_y_vs_xi.GetHistogram());
    ((TPad*)c.GetPad(2))->SetGrid(1, 1);
    c.Save("pdf");
  }

  return 0;
}
