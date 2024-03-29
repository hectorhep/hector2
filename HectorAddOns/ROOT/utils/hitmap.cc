/*
 *  Hector: a beamline propagation tool
 *  Copyright (C) 2016-2023  Laurent Forthomme
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <TH1.h>
#include <TH2.h>

#include "Hector/Beamline.h"
#include "Hector/IO/TwissHandler.h"
#include "Hector/ParticleStoppedException.h"
#include "Hector/Propagator.h"
#include "Hector/Utils/ArgsParser.h"
#include "Hector/Utils/BeamProducer.h"
#include "HectorAddOns/ROOT/Canvas.h"
#include "HectorAddOns/ROOT/DrawUtils.h"

using namespace std;

int main(int argc, char* argv[]) {
  string twiss_filename, interaction_point;
  unsigned int num_particles;
  double crossing_angle_x, crossing_angle_y;
  double hitmaps_dist = -1.;
  double beam_angular_divergence_ip, beam_lateral_width_ip, particles_energy;
  double s_pos;

  hector::ArgsParser args(
      argc,
      argv,
      {{"twiss-file", "Twiss file", &twiss_filename}, {"s-pos", "s-coordinate (m)", &s_pos}},
      {{"show-every", "s-step between two hitmaps", -1, &hitmaps_dist},
       {"num-parts", "number of particles to generate", 1000, &num_particles},
       {"ip-name", "name of the interaction point", "IP5", &interaction_point},
       {"xingangle-x", "crossing angle in the x direction (rad)", 180.e-6, &crossing_angle_x},
       {"xingangle-y", "crossing angle in the y direction (rad)", 0., &crossing_angle_y},
       {"angular-divergence",
        "beam angular divergence at the interaction point (rad)",
        30.23e-6,
        &beam_angular_divergence_ip},
       {"--beam-width", "beam lateral width at the interaction point (m)", 13.63e-6, &beam_lateral_width_ip},
       {"--particles-energy", "beam particles energy (GeV)", 6500., &particles_energy}});
  hector::io::Twiss parser(twiss_filename, interaction_point, s_pos);
  parser.printInfo();

  //const hector::TwoVector offset( -0.097, 0. );
  const hector::TwoVector offset(0., 0.);
  parser.beamline()->offsetElementsAfter(120., offset);

  hector::Propagator prop(parser.beamline());

  //TH2D hitmap( "hitmap", "x (mm)@@y (mm)", 200, -10., 10., 200, -10., 10. );
  //TH2D hitmap( "hitmap", "x (mm)@@y (mm)", 200, -5., 5., 200, -5., 5. );
  TH2D hitmap("hitmap", "x (mm)@@y (mm)", 200, -25., 25., 200, -50., 50.);
  //TH2D hitmap( "hitmap", "x (mm)@@y (mm)", 200, -1., 1., 200, -1., 1. );
  //TH2D hitmap( "hitmap", "x (mm)@@y (mm)", 200, 80., 120., 200, -50., 50. );
  map<float, TH2D*> m_hitmaps;
  if (hitmaps_dist > 0.) {
    double s = 0.;
    while (s <= s_pos) {
      m_hitmaps[s] = dynamic_cast<TH2D*>(hitmap.Clone(Form("hitmap_s%.3f", s)));
      s += hitmaps_dist;
    }
  }

  hector::beam::GaussianParticleGun gun;
  //gun.setElimits( particles_energy*0.95, particles_energy );
  gun.setElimits(particles_energy);
  gun.setXparams(0., beam_lateral_width_ip);
  gun.setYparams(0., beam_lateral_width_ip);
  gun.setTXparams(crossing_angle_x, beam_angular_divergence_ip);
  gun.setTYparams(crossing_angle_y, beam_angular_divergence_ip);
  //hector::BeamProducer::Xscanner gun( num_particles, hector::Parameters::get()->beamEnergy(), 0., 0.01 );

  unsigned short num_stopped = 0;
  map<const hector::element::ElementPtr, unsigned short> stopped_at;
  for (size_t i = 0; i < num_particles; ++i) {
    if ((int)(i * (double)num_particles / num_particles) % 1000 == 0)
      H_INFO << ">>> Generating particle " << i << " / " << num_particles;

    // propagation through the beamline
    hector::Particle p = gun.shoot();
    try {
      prop.propagate(p, s_pos);
      //p.dump();
      //if ( prop.stopped( p, s_pos ) ) { /*cout << "prout" << endl;*/ num_stopped++; continue; }
      const hector::TwoVector pos(p.stateVectorAt(s_pos).position() - offset);
      H_INFO << s_pos << " -> " << pos;
      hitmap.Fill(pos.x() * 1.e3, pos.y() * 1.e3);
    } catch (hector::ParticleStoppedException& pse) {
      //pse.dump();
      stopped_at[pse.stoppingElement()]++;
      num_stopped++;
    }
    // propagate at every position step
    for (auto& hm : m_hitmaps) {
      try {
        p.clear();
        prop.propagate(p, hm.first);
        const hector::TwoVector pos(p.stateVectorAt(hm.first).position() - offset);
        hm.second->Fill(pos.x(), pos.y());
      } catch (hector::Exception& e) {
        continue;
      }
    }
  }
  H_INFO.log([&](auto& log) {
    log << Form("%.1f%% of particles (%d/%d) stopped before s = %.2f",
                100. * num_stopped / num_particles,
                num_stopped,
                num_particles,
                s_pos);
    for (const auto& se : stopped_at) {
      log << "\n\t>> "
          << Form("%.1f%% of particles (%d/%d) stopped in ", 100. * se.second / num_particles, se.second, num_particles)
          << se.first->name() << " " << se.first->typeName();
    }
  });

  const string top_label = Form("s = %.2f m, #alpha_{X} = %.1f #murad", s_pos, crossing_angle_x * 1.e6);
  {
    hector::Canvas c("hitmap", top_label.c_str());
    hitmap.Draw("colz");
    c.Prettify(&hitmap);

    hector::Canvas::PaveText(
        0.01,
        0.,
        0.05,
        0.05,
        Form("#scale[0.3]{Beamline: %s}", twiss_filename.substr(twiss_filename.find_last_of("/\\") + 1).c_str()))
        .Draw();

    c.Save("pdf");
  }
  if (m_hitmaps.size() > 0) {
    hector::Canvas c("hitmap_combined", top_label.c_str());
    unsigned int num_rows = sqrt(m_hitmaps.size()),
                 num_cols = m_hitmaps.size() / num_rows + (m_hitmaps.size() % num_rows == 0 ? 0 : 1);
    c.Divide(num_rows, num_cols);
    c.Pad()->SetTopMargin(0.2);
    unsigned short i = 0;
    for (auto& hm : m_hitmaps) {
      c.cd(i + 1);
      hm.second->Draw("colz");
      c.Prettify(hm.second);
      i++;
    }
    c.Save("pdf");
  }

  return 0;
}
