#ifndef Hector_Utils_Kinematics_h
#define Hector_Utils_Kinematics_h

namespace hector {
  class Particle;
  /// Let the particle emit a photon
  void emitGamma(Particle& part_in, double e_gamma, double q2_gamma, double phi_min, double phi_max);
  /// Convert a particle energy to its momentum loss
  double e_to_xi(double energy, double e0 = -1.);
  /// Convert a particle momentum loss to its energy
  double xi_to_e(double xi, double e0 = -1.);
}  // namespace hector

#endif
