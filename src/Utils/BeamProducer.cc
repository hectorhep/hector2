#include "Hector/Utils/BeamProducer.h"
#include "Hector/Utils/Kinematics.h"
#include "Hector/Exception.h"

#include <CLHEP/Random/RandFlat.h>
#include <CLHEP/Random/RandGauss.h>

namespace hector {
  beam::Xiscanner::Xiscanner(unsigned short num_part, float xi_min, float xi_max, float x, float y, float s_ini)
      : LinearScanner(num_part, x, x, y, y, xi_to_e(xi_min), xi_to_e(xi_max), s_ini) {}

  unsigned short beam::LinearScanner::LinearScanner::next() {
    if (num_gen_ >= num_part_)
      throw H_ERROR << "Too much particles already generated!";
    return num_gen_++;
  }

  Particle beam::Xscanner::shoot() {
    const TwoVector pos_ini(p1_.first + LinearScanner::next() * (p1_.second - p1_.first) / (num_part_ - 1), p2_.first);
    const LorentzVector mom_ini(0., 0., 0., Parameters::get()->beamEnergy());
    return Particle(StateVector(mom_ini, pos_ini), s_.first);
  }

  Particle beam::Yscanner::shoot() {
    const TwoVector pos_ini(p2_.first, p1_.first + LinearScanner::next() * (p1_.second - p1_.first) / (num_part_ - 1));
    const LorentzVector mom_ini(0., 0., 0., Parameters::get()->beamEnergy());
    return Particle(StateVector(mom_ini, pos_ini), s_.first);
  }

  Particle beam::TXscanner::shoot() {
    const TwoVector angles(p1_.first + LinearScanner::next() * (p1_.second - p1_.first) / (num_part_ - 1), p2_.first);
    return Particle(StateVector(TwoVector(), angles, e_.first), s_.first);
  }

  Particle beam::TYscanner::shoot() {
    const TwoVector angles(p2_.first, p1_.first + LinearScanner::next() * (p1_.second - p1_.first) / (num_part_ - 1));
    return Particle(StateVector(TwoVector(), angles, e_.first), s_.first);
  }

  Particle beam::Xiscanner::shoot() {
    const double energy = e_.first + LinearScanner::next() * (e_.second - e_.first) / (num_part_ - 1);
    const double mom = sqrt(energy * energy - pow(Parameters::get()->beamParticlesMass(), 2));
    return Particle(StateVector(LorentzVector(0., 0., mom, energy), TwoVector(p1_.first, p2_.first)), s_.first);
  }

  beam::params_t beam::GaussianParticleGun::parameters(float lim1, float lim2) {
    return beam::params_t(0.5 * (lim1 + lim2), 0.5 * (lim2 - lim1));
  }

  void beam::GaussianParticleGun::smearXi(float xi_mean, float xi_sigma) {
    setEparams(xi_to_e(xi_mean), xi_to_e(xi_sigma));
  }
}  // namespace hector
