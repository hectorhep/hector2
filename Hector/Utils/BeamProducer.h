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

#ifndef Hector_Utils_BeamProducer
#define Hector_Utils_BeamProducer

#include <random>
#include <vector>

#include "Hector/Parameters.h"
#include "Hector/Particle.h"

namespace hector {
  /// Generator for beam of particles
  namespace beam {
    /// Lower and upper limits
    typedef std::pair<float, float> params_t;
    /// Generic scanner for a given granularity of a parameter
    class LinearScanner {
    public:
      /// Class constructor
      LinearScanner(unsigned short num_part,
                    float p1_ini,
                    float p1_end,
                    float p2_ini,
                    float p2_end = 999.,
                    float e_ini = 0.,
                    float e_end = 0.,
                    float s_ini = 0.)
          : num_part_(num_part),
            num_gen_(0),
            p1_(p1_ini, (p1_end == 999.) ? p1_ini : p1_end),
            p2_(p2_ini, (p2_end == 999.) ? p2_ini : p2_end),
            e_(e_ini, e_end),
            s_(s_ini, s_ini) {}
      /// Get the next event number to scan
      unsigned short next();
      /// Number of particles to generate to perform a full scan
      unsigned short numScanParticles() const { return num_part_; }
      /// Generate a new particle
      virtual Particle shoot() = 0;

    protected:
      /// Number of particles to generate to perform a full scan
      unsigned short num_part_;
      /// Number of particles already generated in the scan
      unsigned short num_gen_;
      /// Lower and upper limits for the first scan parameter
      params_t p1_;
      /// Lower and upper limits for the second scan parameter
      params_t p2_;
      /// Lower and upper limits on the beam energy
      params_t e_;
      /// Lower and upper limits for the s coordinate to generate
      params_t s_;
    };

    /// Beam of particles to scan the optics following the x axis
    class Xscanner : public LinearScanner {
    public:
      /// Class constructor
      /// \param[in] num_part number of particles to generate for a full scan
      /// \param[in] e_ini initial energy for the particles
      /// \param[in] x_min minimal parameter value
      /// \param[in] x_max maximal parameter value
      /// \param[in] y fixed parameter value
      /// \param[in] s_ini initial s position
      Xscanner(unsigned short num_part, float e_ini, float x_min, float x_max, float y = 0., float s_ini = 0.)
          : LinearScanner(num_part, x_min, x_max, y, y, e_ini, e_ini, s_ini) {}
      Particle shoot() override;
    };

    /// Beam of particles to scan the optics following the y axis
    class Yscanner : public LinearScanner {
    public:
      /// Class constructor
      /// \param[in] num_part number of particles to generate for a full scan
      /// \param[in] e_ini initial energy for the particles
      /// \param[in] x fixed parameter value
      /// \param[in] y_min minimal parameter value
      /// \param[in] y_max maximal parameter value
      /// \param[in] s_ini initial s position
      Yscanner(unsigned short num_part, float e_ini, float y_min, float y_max, float x = 0., float s_ini = 0.)
          : LinearScanner(num_part, y_min, y_max, x, x, e_ini, e_ini, s_ini) {}
      Particle shoot() override;
    };

    /// Beam of particles to scan the optics with respect to the x angle
    class TXscanner : public LinearScanner {
    public:
      /// Class constructor
      /// \param[in] num_part number of particles to generate for a full scan
      /// \param[in] e_ini initial energy for the particles
      /// \param[in] tx_min minimal parameter value
      /// \param[in] tx_max maximal parameter value
      /// \param[in] ty fixed parameter value
      /// \param[in] s_ini initial s position
      TXscanner(unsigned short num_part, float e_ini, float tx_min, float tx_max, float ty = 0., float s_ini = 0.)
          : LinearScanner(num_part, tx_min, tx_max, ty, ty, e_ini, e_ini, s_ini) {}
      Particle shoot() override;
    };

    /// Beam of particles to scan the optics with respect to the y angle
    class TYscanner : public LinearScanner {
    public:
      /// Class constructor
      /// \param[in] num_part number of particles to generate for a full scan
      /// \param[in] e_ini initial energy for the particles
      /// \param[in] tx fixed parameter value
      /// \param[in] ty_min minimal parameter value
      /// \param[in] ty_max maximal parameter value
      /// \param[in] s_ini initial s position
      TYscanner(unsigned short num_part, float e_ini, float ty_min, float ty_max, float tx = 0., float s_ini = 0.)
          : LinearScanner(num_part, ty_min, ty_max, tx, tx, e_ini, e_ini, s_ini) {}
      Particle shoot() override;
    };
    /// Beam of particles to scan the optics with respect to the longitudinal momentum loss
    class Xiscanner : public LinearScanner {
    public:
      /// Class constructor
      /// \param[in] num_part number of particles to generate for a full scan
      /// \param[in] xi_min minimal parameter value
      /// \param[in] xi_max maximal parameter value
      /// \param[in] x horizontal particle position
      /// \param[in] y vertical particle position
      /// \param[in] s_ini initial s position
      Xiscanner(unsigned short num_part, float xi_min, float xi_max, float x = 0., float y = 0., float s_ini = 0.);
      Particle shoot() override;
    };

    /// A generic templated particle gun
    template <class T>
    class ParticleGun {
    public:
      /// Class constructor
      ParticleGun()
          : e_(parameters(Parameters::get().beamEnergy(), Parameters::get().beamEnergy())),
            s_(parameters(0., 0.)),
            x_(parameters(0., 0.)),
            y_(parameters(0., 0.)),
            tx_(parameters(0., 0.)),
            ty_(parameters(0., 0.)),
            mass_(Parameters::get().beamParticlesMass()),
            charge_(Parameters::get().beamParticlesCharge()) {
        rngs_[0] = T(s_.first, s_.second);
        rngs_[1] = T(x_.first, x_.second);
        rngs_[2] = T(y_.first, y_.second);
        rngs_[3] = T(tx_.first, tx_.second);
        rngs_[4] = T(ty_.first, ty_.second);
        rngs_[5] = T(e_.first, e_.second);
      }

      /// Generate one particle according to the templated distribution
      Particle shoot() {
        StateVector vec;
        const float s = rngs_[0](gen_), x = rngs_[1](gen_), y = rngs_[2](gen_), tx = rngs_[3](gen_),
                    ty = rngs_[4](gen_), e = rngs_[5](gen_);
        vec.setPosition(TwoVector(x, y));
        vec.setAngles(TwoVector(tx, ty));
        vec.setM(mass_);
        vec.setEnergy(e);

        Particle p(vec, s);
        p.setCharge(charge_);
        return p;
      }

      //----- Full beam information

      /// Set the parameters to the initial beam energy distribution
      void setEparams(float e1, float e2) { e_ = params_t(e1, e2); }
      /// Set the lower and upper limits to the initial beam energy distribution
      void setElimits(float e1, float e2 = -1.) {
        if (e2 < 0)
          e2 = e1;  // energies are supposingly positive
        e_ = parameters(e1, e2);
      }

      /// Set the parameters to the initial longitudinal beam position distribution
      void setSparams(float s1, float s2) { s_ = params_t(s1, s2); }
      /// Set the lower and upper limits to the initial longitudinal beam position distribution
      void setSlimits(float s1, float s2) { s_ = parameters(s1, s2); }

      /// Set the parameters to the horizontal beam position distribution
      void setXparams(float x1, float x2) { x_ = params_t(x1, x2); }
      /// Set the lower and upper limits to the horizontal beam position distribution
      void setXlimits(float x1, float x2) { x_ = parameters(x1, x2); }

      /// Set the parameters to the vertical beam position distribution
      void setYparams(float y1, float y2) { y_ = params_t(y1, y2); }
      /// Set the lower and upper limits to the vertical beam position distribution
      void setYlimits(float y1, float y2) { y_ = parameters(y1, y2); }

      /// Set the parameters to the horizontal angular distribution
      void setTXparams(float tx1, float tx2) { tx_ = params_t(tx1, tx2); }
      /// Set the lower and upper limits to the horizontal angular distribution
      void setTXlimits(float tx1, float tx2) { tx_ = parameters(tx1, tx2); }

      /// Set the parameters to the vertical angular distribution
      void setTYparams(float ty1, float ty2) { ty_ = params_t(ty1, ty2); }
      /// Set the lower and upper limits to the vertical angular parameter distribution
      void setTYlimits(float ty1, float ty2) { ty_ = parameters(ty1, ty2); }

      //----- Single particle information

      /// Outgoing particles' mass (in GeV)
      float particleMass() const { return mass_; }
      /// Set the outgoing particles' mass (in GeV)
      void setParticleMass(float mass) { mass_ = mass; }
      /// Outgoing particles' charge (in e)
      float particleCharge() const { return charge_; }
      /// Set the outgoing particles' charge (in e)
      void setParticleCharge(float q) { charge_ = q; }

    private:
      /// Translate lower and upper limits into parameters to give to the random generator
      params_t parameters(float lim1, float lim2) { return params_t(lim1, lim2); }

      std::array<T, 6> rngs_;
      std::default_random_engine gen_;
      params_t e_, s_;
      params_t x_, y_;
      params_t tx_, ty_;
      float mass_, charge_;
    };

    namespace rnd {
      struct Uniform : std::uniform_real_distribution<double> {
        explicit Uniform(double min = 0., double max = 1.) : std::uniform_real_distribution<double>(min, max) {}
      };
      struct Gaussian : std::normal_distribution<double> {
        explicit Gaussian(double mean = 0., double stddev = 1.) : std::normal_distribution<double>(mean, stddev) {}
      };
    }  // namespace rnd
    /// Beam of particles with flat s, x, y, Tx, Ty and energy distributions
    typedef ParticleGun<rnd::Uniform> FlatParticleGun;
    /// Beam of particles with gaussian s, x, y, Tx, Ty and energy distributions
    struct GaussianParticleGun : ParticleGun<rnd::Gaussian> {
      using ParticleGun::ParticleGun;
      /// Specialization for Gaussian parameters
      params_t parameters(float lim1, float lim2);
      void smearX(float x_mean, float x_sigma) { setXparams(x_mean, x_sigma); }
      void smearY(float y_mean, float y_sigma) { setYparams(y_mean, y_sigma); }
      void smearTx(float tx_mean, float tx_sigma) { setTXparams(tx_mean, tx_sigma); }
      void smearTy(float ty_mean, float ty_sigma) { setTYparams(ty_mean, ty_sigma); }
      void smearEnergy(float e_mean, float e_sigma) { setEparams(e_mean, e_sigma); }
      void smearXi(float xi_mean, float xi_sigma);
    };
  }  // namespace beam
}  // namespace hector

#endif
