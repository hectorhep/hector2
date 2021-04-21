#include "Hector/Utils/Kinematics.h"

#include <CLHEP/Random/RandFlat.h>

#include "Hector/Parameters.h"
#include "Hector/Exception.h"
#include "Hector/Particle.h"

namespace hector {
  void emitGamma(Particle& part, double e_gamma, double q2_gamma, double phi_min, double phi_max) {
    const double pos_ini = part.firstS();
    auto& sv_ini = part.firstStateVector();

    if (q2_gamma == 0.) {
      H_INFO << "Virtuality is null: only energy has changed.";
      sv_ini.setEnergy(sv_ini.energy() - e_gamma);
      return;
    }
    const double m1 = Parameters::get()->beamParticlesMass(), e1 = sv_ini.energy(),
                 e2 = sv_ini.energy() - e_gamma;  // particle energy : before (1) / after (2)

    const double p1 = sqrt(e1 * e1 - m1 * m1),
                 p2 = sqrt(e2 * e2 - m1 * m1);  // particle momentum : before (1) / after (2)
    const double q2min = e_gamma * e_gamma - pow(p1 + p2,
                                                 2);  // lower bound from kinematics E^2 - (p1 + p2)^2
    const double q2max =
        -2 * pow(m1 * e_gamma / (p1 + p2), 2) * (1. + (e1 * e1 + e2 * e2 - m1 * m1) / (e1 * e2 + p1 * p2));
    // upper bound from kinematics ; E^2 - (p1-p2)^2; is bad for numerical computations

    // if q2 is not within [q2min, q2max] there will be mathematical problems (like cos(eta) > 1).
    // So if q2 > q2max, we force q2 = q2max (-> cos(eta) = 1) and if q2 < q2min,
    // we force q2 = q2min (-> cos(eta) = 1) BUT the user knows something was wrong with
    // the value of "Particle::physical()"
    if (q2_gamma > q2max || q2_gamma < q2min) {
      H_WARNING << "Non physical particle! Q2=" << q2_gamma << " GeV^2 and E=" << e_gamma
                << "%.5f GeV are not compatible.";
      part.setPhysical(false);
    }

    const double q2 = std::max(q2min, std::min(q2max, q2_gamma));

    sv_ini.setEnergy(e2);
    // gkk is k
    const double gkk = sqrt(e_gamma * e_gamma - q2);
    // eta is the angle between gamma and initial direction of the gamma-emitting
    // particle ceta = cos(eta) and seta = sin(eta)

    const double ceta = sqrt(1. + pow(m1 / p1, 2)) * sqrt(1. + q2 / gkk * gkk) - q2 / (2 * p1 * gkk),
                 seta = sqrt(1. - ceta * ceta);
    // theta is the angle between particle and beam
    const double theta = atan(seta / (Parameters::get()->beamEnergy() / gkk - ceta)),
                 phi = CLHEP::RandFlat::shoot(phi_min, phi_max);

    TwoVector old_ang(sv_ini.angles());
    sv_ini.setAngles(old_ang + TwoVector(theta * cos(phi), -theta * sin(phi)));

    // caution: emitting a photon erases all known positions !
    part.clear();
    part.addPosition(pos_ini, sv_ini);
  }

  double e_to_xi(double energy, double e0) {
    if (e0 < 0.)
      e0 = Parameters::get()->beamEnergy();
    return 1. - energy / e0;
  }

  double xi_to_e(double xi, double e0) {
    if (e0 < 0.)
      e0 = Parameters::get()->beamEnergy();
    return e0 * (1. - xi);
  }
}  // namespace hector
