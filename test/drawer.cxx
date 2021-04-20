#include "Hector/Beamline.h"
#include "Hector/IO/TwissHandler.h"
#include "Hector/Core/ParticleStoppedException.h"
#include "Hector/Propagator/Propagator.h"

#include "Hector/Utils/ArgsParser.h"
#include "Hector/Utils/BeamProducer.h"
#include "Hector/Utils/Timer.h"

#include "utils.h"
#include "Canvas.h"

#include <CLHEP/Random/RandGauss.h>
#include <CLHEP/Units/SystemOfUnits.h>

#include "TGraph.h"
#include "THStack.h"
#include "TH2.h"
#include "TMultiGraph.h"
#include "TAxis.h"
#include "TStyle.h"

using namespace std;

TGraphErrors mean_trajectory(const TMultiGraph&);

int main(int argc, char* argv[]) {
  //hector::Parameters::get()->setLoggingThreshold( hector::Info );

  vector<string> twiss_filenames, meas_filenames;
  vector<int> colours;
  string ip_name;
  vector<double> crossing_angles_x, crossing_angles_y, offset;
  double max_s;
  double beam_lateral_width_ip, beam_angular_divergence_ip;
  double xi;
  double scale_x, scale_y;
  unsigned int num_particles;
  bool show_paths = false, dipoles_enable, draw_monitors = false, dump_beamlines;

  hector::ArgsParser(
      argc,
      argv,
      {
          {"twiss-files", "beamline(s) Twiss file(s)", &twiss_filenames, 'i'},
      },
      {
          {"ip-name", "name of the interaction point", "IP5", &ip_name, 'c'},
          {"max-s", "maximal s-coordinate (m)", 250., &max_s},
          {"num-parts", "number of particles to generate", 2000, &num_particles, 'n'},
          {"xi", "particles momentum loss", 0.1, &xi},
          {"offset", "beam position at bunch crossing (m)", vector<double>(2, 0.), &offset, 'o'},
          {"alpha-x", "crossing angle in the x direction (rad)", vector<double>(2, 180.e-6), &crossing_angles_x, 'x'},
          {"alpha-y", "crossing angle in the y direction (rad)", vector<double>(2, 0.), &crossing_angles_y, 'y'},
          {"scale-x", "horizontal coordinate scaling (m)", 0.1, &scale_x},
          {"scale-y", "vertical coordinate scaling (m)", 0.05, &scale_y},
          {"beam-divergence", "beam angular divergence (rad)", 30.23e-6, &beam_angular_divergence_ip, 'r'},
          {"beam-width", "beam transverse width (m)", 16.63e-6, &beam_lateral_width_ip, 'w'},
          {"show-paths", "show individual particle paths", false, &show_paths},
          {"simulate-dipoles", "Simulate the dipole effects?", true, &dipoles_enable},
          {"meas-files", "list of measurements files", vector<string>{}, &meas_filenames, 'm'},
          {"draw-monitors", "show monitors output", false, &draw_monitors},
          {"colours", "beam colours", vector<int>{kBlue + 1, kRed + 1}, &colours},
          {"dump-beamlines", "dump beamlines in terminal", false, &dump_beamlines, 'd'},
      });

  if (offset.size() < 1)
    throw logic_error("Beam position at bunch crossing is invalid!");
  else if (offset.size() < 2)
    offset.emplace_back(offset.at(0));

  //for ( const auto& x : crossing_angles_x ) cout << x << endl;
  //--- general propagation parameters
  //hector::Parameters::get()->setComputeApertureAcceptance( false ); //FIXME
  //hector::Parameters::get()->setEnableKickers( false ); //FIXME
  hector::Parameters::get()->setEnableDipoles(dipoles_enable);

  //--- define the propagator objects
  vector<hector::Propagator> propagators;

  vector<TMultiGraph*> mg_x, mg_y;
  vector<std::unique_ptr<TH1D> > h_timing;
  vector<map<hector::element::ElementBase*, std::unique_ptr<TH2D> > > monitors_plts(twiss_filenames.size());

  for (const auto& fn : twiss_filenames)
    h_timing.emplace_back(new TH1D(("timing" + fn).c_str(), "Propagation time@@Event@@ms?.2f", 100, 0., 10.));

  unsigned short i = 0;
  for (const auto& fn : twiss_filenames) {
    if (fn == "")
      continue;
    const hector::io::Twiss parser(fn, ip_name, max_s);
    //parser.beamline()->offsetElementsAfter( 120., CLHEP::Hep2Vector( 0.097, 0. ) );
    //parser.beamline()->offsetElementsAfter( 120., CLHEP::Hep2Vector( +0.097, 0. ) );
    //parser.printInfo();
    if (!parser.beamline())
      throw runtime_error("Failed to parse the beamline!");
    auto bl = new hector::Beamline(*parser.beamline());

    propagators.emplace_back(bl);
    mg_x.emplace_back(new TMultiGraph);
    mg_y.emplace_back(new TMultiGraph);

    if (draw_monitors)
      for (const auto& elemPtr : bl->elements())
        if (elemPtr->type() == hector::element::aMonitor)
          monitors_plts[i][elemPtr.get()].reset(
              new TH2D(Form("bl%d_%s", i, elemPtr->name().c_str()),
                       Form("%s (s=%.2fm);x (mm);y (mm)", elemPtr->name().c_str(), elemPtr->s()),
                       100,
                       -25.,
                       25.,
                       100,
                       -25.,
                       25.));

    //--- look at the beamline(s)
    if (dump_beamlines)
      bl->dump();

    auto rps = bl->find("XRPH\\.");
    cout << "---> beamline " << fn << " has " << rps.size() << " horizontal Roman pots!" << endl;
    for (const auto& rp : rps)
      cout << " >> Roman pot " << rp->name() << " at s=" << rp->s() << " m" << endl;
    ++i;
  }

  hector::beam::GaussianParticleGun gun;
  gun.smearX(offset.at(0), beam_lateral_width_ip);
  gun.smearY(offset.at(1), beam_lateral_width_ip);

  hector::Timer tmr;

  for (size_t i = 0; i < num_particles; ++i) {
    if (num_particles > 10 && i % (num_particles / 10) == 0)
      cout << "> event " << i << " / " << num_particles << endl;

    unsigned short j = 0;
    for (const auto& prop : propagators) {
      const double crossing_angle_x =
          (crossing_angles_x.size() == propagators.size()) ? crossing_angles_x.at(j) : crossing_angles_x.at(0);
      const double crossing_angle_y =
          (crossing_angles_y.size() == propagators.size()) ? crossing_angles_y.at(j) : crossing_angles_y.at(0);
      gun.smearTx(crossing_angle_x, beam_angular_divergence_ip);
      gun.smearTy(crossing_angle_y, beam_angular_divergence_ip);
      //gun.smearTy( crossing_angle_y, 0. );
      hector::Particle p = gun.shoot();
      p.firstStateVector().setXi(xi);
      //      p.firstStateVector().setKick( 0. );
      //      cout << p.firstStateVector().kick() << endl;
      //----- beamline propagation
      TGraph gr_x, gr_y;  // 1 graph for each trajectory
      tmr.reset();
      try {
        prop.propagate(p, max_s);
      } catch (hector::ParticleStoppedException& e) {
      }
      h_timing[j]->Fill(tmr.elapsed() * 1.e3);  // in ms
      for (const auto& pos : p) {
        gr_x.SetPoint(gr_x.GetN(), pos.first, pos.second.position().x());
        gr_y.SetPoint(gr_y.GetN(), pos.first, pos.second.position().y());
      }
      if (draw_monitors) {
        for (const auto& elemPtr : prop.beamline()->elements()) {
          if (elemPtr->type() != hector::element::aMonitor)
            continue;
          try {
            const auto sv_mon = p.stateVectorAt(elemPtr->s() + 0.5 * elemPtr->length());
            monitors_plts[j][elemPtr.get()]->Fill(sv_mon.x() * 1.e3, sv_mon.y() * 1.e3);
          } catch (hector::Exception&) {
          }
        }
      }
      gr_x.SetLineColorAlpha(colours.at(j), 0.05);
      gr_y.SetLineColorAlpha(colours.at(j), 0.05);
      gr_x.SetLineWidth(1);
      gr_y.SetLineWidth(1);
      mg_x[j]->Add((TGraph*)gr_x.Clone());
      mg_y[j]->Add((TGraph*)gr_y.Clone());
      ++j;
    }
  }

  // drawing part

  {
    ostringstream os_xa;
    for (unsigned short i = 0; i < crossing_angles_x.size(); ++i) {
      if (i > 0)
        os_xa << "/";
      os_xa << Form("%.1f", crossing_angles_x.at(i) * 1.e6);
    }
    hector::Canvas c("beamline",
                     Form("E_{p} = %.1f TeV, #alpha_{X} = %s #murad",
                          hector::Parameters::get()->beamEnergy() * CLHEP::GeV / CLHEP::TeV,
                          os_xa.str().c_str()),
                     true);
    c.SetWindowSize(800, 800);
    //c.SetGrayscale();

    const bool draw_apertures = true;

    TGraph gr_bpm_meas_x[5], gr_rp_meas_x[5];
    unsigned short ifn = 0;
    for (const auto& fn : meas_filenames) {
      ifstream mf(fn);
      cout << fn << endl;
      while (!mf.eof()) {
        vector<string> fields;
        string str;
        getline(mf, str);
        istringstream iss(str);
        if (str[0] == '#')
          continue;
        unsigned short j = 0;
        while (iss >> str) {
          if (j > 3)
            break;
          fields.emplace_back(str);
          ++j;
        }
        if (fields.size() == 0)
          continue;
        switch (stoi(fields.at(0))) {
          case 1:  // BPM
            if (stod(fields.at(2)) != -1.)
              gr_bpm_meas_x[ifn].SetPoint(gr_bpm_meas_x[ifn].GetN(), stod(fields.at(1)), +stod(fields.at(2)) * 1.e-3);
            if (stod(fields.at(3)) != -1.)
              gr_bpm_meas_x[ifn].SetPoint(gr_bpm_meas_x[ifn].GetN(), stod(fields.at(1)), -stod(fields.at(3)) * 1.e-3);
            break;
          case 2:  // RP
            if (stod(fields.at(2)) != -1.)
              gr_rp_meas_x[ifn].SetPoint(gr_rp_meas_x[ifn].GetN(), stod(fields.at(1)), +stod(fields.at(2)) * 1.e-3);
            if (stod(fields.at(3)) != -1.)
              gr_rp_meas_x[ifn].SetPoint(gr_rp_meas_x[ifn].GetN(), stod(fields.at(1)), -stod(fields.at(3)) * 1.e-3);
            break;
        }
      }
      ++ifn;
    }

    struct plot_t {
      char axis;
      vector<TMultiGraph*>& gr;
      double scale;
    };

    unsigned short i = 0;
    TMultiGraph* mg[2];
    for (auto& plt : vector<plot_t>{{'x', mg_x, scale_x}, {'y', mg_y, scale_y}}) {
      c.cd(i + 1);
      mg[i] = new TMultiGraph;
      for (unsigned short j = 0; j < propagators.size(); ++j) {
        TGraphErrors g_mean = mean_trajectory(*plt.gr[j]);
        g_mean.SetLineColor(colours.at(j));
        g_mean.SetFillColorAlpha(colours.at(j), 0.33);
        if (show_paths)
          mg[i]->Add(plt.gr[j]);
        mg[i]->Add((TGraph*)g_mean.Clone());
      }
      mg[i]->Draw("a2");
      mg[i]->SetTitle(Form(".@@%c (m)", plt.axis));
      mg[i]->GetXaxis()->SetLimits(0., max_s);
      mg[i]->GetYaxis()->SetRangeUser(-plt.scale, plt.scale);
      c.Prettify(mg[i]->GetHistogram());

      unsigned short j = 0;
      for (const auto& prop : propagators)
        drawBeamline(plt.axis, prop.beamline(), j++, ip_name.c_str(), plt.scale, 0., max_s, draw_apertures);

      if (i == 0)
        for (unsigned short j = 0; j < meas_filenames.size(); ++j) {
          gr_bpm_meas_x[j].SetMarkerStyle(26);
          gr_bpm_meas_x[j].Draw("p,same");
          gr_rp_meas_x[j].SetMarkerStyle(20);
          gr_rp_meas_x[j].Draw("p,same");
        }

      mg[i]->Draw("l3");
      mg[i]->GetXaxis()->SetLabelSize(0.055);
      if (i > 0)
        mg[i]->GetXaxis()->SetTitleOffset(0.7);
      mg[i]->GetYaxis()->SetLabelSize(0.055);
      mg[i]->GetYaxis()->SetTitleSize(0.1);
      mg[i]->GetYaxis()->SetTitleOffset(0.65);
      mg[i]->GetXaxis()->SetTitle("");
      c.GetPad(i + 1)->SetGrid(1, 1);
      ++i;
    }

    c.cd();
    ostringstream os;
    unsigned short j = 1;
    for (const auto& fn : twiss_filenames) {
      os << ((j == 1) ? "" : " - ") << "#color[" << colours[j - 1] << "]{Beam " << j
         << "}: " << Form("%s", fn.substr(fn.find_last_of("/\\") + 1).c_str());
      ++j;
    }
    const char* label = Form("#scale[0.33]{%s}", os.str().c_str());
    hector::Canvas::PaveText pt(0.0, 0.0, 0.15, 0.01, label);
    pt.SetTextAlign(kHAlignLeft + kVAlignBottom);
    pt.Draw();

    c.Save("pdf");
    for (unsigned short k = 0; k < i; ++k)
      delete mg[k];
  }
  {  // draw the legend
    hector::Canvas c("beamline_legend", "", false, false);
    elementsLegend leg;
    gStyle->SetOptStat(0);
    for (const auto& prop : propagators)
      leg.feedBeamline(prop.beamline());
    leg.Draw();
    c.Save("pdf");
  }
  if (draw_monitors) {
    unsigned short i = 0;
    for (auto& mp : monitors_plts) {
      hector::Canvas c(Form("monitors_beam%d", i + 1), Form("Beam %d", i + 1));
      const unsigned short num_x = ceil(sqrt(mp.size())), num_y = ceil(mp.size() * 1. / num_x);
      c.Divide(num_x, num_y);
      unsigned short j = 0;
      for (auto& plt : mp) {
        c.cd(++j);
        plt.second->Draw("colz");
        //c.Prettify( plt.second );
      }
      c.Save("pdf");
      ++i;
    }
  }
  {
    hector::Canvas c("propagation_time", Form("%d events, s_{max} = %.1f m", num_particles, max_s));
    gStyle->SetOptStat(1111);
    THStack hs;
    unsigned short i = 0;
    for (auto& h : h_timing) {
      if (i == 0)
        hs.SetTitle(h->GetTitle());
      h->SetLineColor(colours.at(i));
      h->SetFillColor(colours.at(i));
      h->SetFillStyle(3004 + i);
      c.AddLegendEntry(h.get(), Form("Beam %d", ++i), "f");
      hs.Add(h.get());
    }
    hs.Draw("h,nostack");
    c.Prettify(hs.GetHistogram());
    hs.SetTitle("");
    c.SetLogy();
    c.Save("pdf");
  }

  return 0;
}

TGraphErrors mean_trajectory(const TMultiGraph& amg) {
  map<double, double> mean_val;
  map<double, unsigned int> num_vals;
  // first loop to extract the mean and number of points
  for (const auto& obj : *amg.GetListOfGraphs()) {
    const auto gr = dynamic_cast<TGraph*>(obj);
    for (int i = 0; i < gr->GetN(); ++i) {
      double x, y;
      gr->GetPoint(i, x, y);
      mean_val[x] += y;
      num_vals[x]++;
    }
  }

  for (auto& v : mean_val)
    v.second /= num_vals[v.first];  // extract the mean

  // second loop to extract the variance
  map<double, double> err_val;

  for (const auto& obj : *amg.GetListOfGraphs()) {
    const auto gr = dynamic_cast<TGraph*>(obj);
    for (int i = 0; i < gr->GetN(); ++i) {
      double x, y;
      gr->GetPoint(i, x, y);
      err_val[x] += pow(y - mean_val.at(x), 2) / num_vals.at(x);
    }
  }

  for (auto& v : err_val)
    v.second = sqrt(v.second);
  assert(err_val.size() == mean_val.size());

  //for ( const auto& v : num_vals ) cout << v.first << "|" << v.second << "|" << mean_val[v.first] << endl;
  TGraphErrors out;
  unsigned short i = 0, j = 0;
  for (const auto& v : mean_val)
    out.SetPoint(i++, v.first, v.second);
  for (const auto& v : err_val)
    out.SetPointError(j++, 0., v.second);
  return out;
}
