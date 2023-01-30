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

#include <iostream>

#include "Hector/Beamline.h"
#include "Hector/Exception.h"
#include "Hector/IO/TwissHandler.h"
#include "Hector/ParticleStoppedException.h"
#include "Hector/Propagator.h"
#include "Hector/Utils/ArgsParser.h"
#include "Hector/Utils/BeamProducer.h"
#include "Hector/Utils/String.h"

using namespace std;

int main(int argc, char* argv[]) {
  string twiss_file, ip;
  double min_s, max_s;
  unsigned int num_part = 100;
  bool shoot;
  hector::ArgsParser(argc,
                     argv,
                     {{"twiss-file", "beamline Twiss file", &twiss_file, 'i'}},
                     {
                         {"interaction-point", "name of the interaction point", "IP5", &ip, 'c'},
                         {"min-s", "minimum arc length s to parse (m)", 0., &min_s},
                         {"max-s", "maximum arc length s to parse (m)", 250., &max_s},
                         {"num-part", "number of particles to shoot", 10, &num_part, 'n'},
                         {"simulate", "simulate a beam propagation", false, &shoot, 's'},
                     });

  hector::io::Twiss parser(twiss_file.c_str(), ip.c_str(), max_s, min_s);
  parser.printInfo();
  H_INFO.log([&](auto& log) {
    log << "\n"
        << "+---------------------+--------------------+----------------------\n"
        << hector::format("| %-19s | %-18s | %20s|\n", "Name", "Type", "Position along s (m)")
        << "+---------------------+--------------------+----------------------\n";

    for (const auto& elem : parser.beamline()->elements()) {
      //if ( elem->type() == hector::element::aDrift ) continue;
      log << hector::format("|%20s | %-18s [ ",
                            elem->name().c_str(),
                            (elem->type() != hector::element::aDrift) ? elem->typeName().c_str() : "");
      string pos_ini = hector::format("%7s", hector::format("%#0.3f", elem->s()).c_str());
      if (elem->length() > 0.)
        log << hector::format(
            "%.7s → %7s m", pos_ini.c_str(), hector::format("%#0.3f", elem->s() + elem->length()).c_str());
      else
        log << hector::format("%17s m", pos_ini.c_str());
      log << " ]\n";
    }
    log << "+---------------------+-------------------------------------------\n";
  });
  //parser.beamline()->dump();

  H_INFO << "beamline matrix at s = " << max_s
         << " m: " << parser.beamline()->matrix(0., hector::Parameters::get().beamParticlesMass(), +1);

  if (shoot) {
    hector::Propagator prop(parser.beamline());
    //parser.beamline()->dump();

    hector::beam::GaussianParticleGun gun;
    gun.smearEnergy(hector::Parameters::get().beamEnergy(), hector::Parameters::get().beamEnergy() * 0.);
    //hector::beam::TXscanner gun(num_part, hector::Parameters::get().beamEnergy(), 0., 1.);
    map<string, unsigned int> stopping_elements;
    for (unsigned int i = 0; i < num_part; ++i) {
      hector::Particle p = gun.shoot();
      p.setCharge(+1);
      try {
        prop.propagate(p, 203.826);
      } catch (const hector::ParticleStoppedException& e) {
        stopping_elements[e.stoppingElement()->name()]++;
        continue;
      } catch (const hector::Exception& e) {
        e.dump(std::cerr);
      }
    }

    H_INFO.log([&](auto& log) {
      log << "Summary\n\t-------";
      for (const auto& el : stopping_elements)
        log << hector::format(
            "\n\t*) %.2f%% of particles stopped in %s", 100. * el.second / num_part, el.first.c_str());
    });
  }

  return 0;
}
