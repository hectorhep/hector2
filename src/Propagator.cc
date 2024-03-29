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

#include <sstream>

#include "Hector/Beamline.h"
#include "Hector/Elements/Element.h"
#include "Hector/Exception.h"
#include "Hector/Parameters.h"
#include "Hector/ParticleStoppedException.h"
#include "Hector/Propagator.h"

namespace hector {
  void Propagator::propagate(Particle& part, double s_max) const {
    part.clear();

    const double energy_loss = (Parameters::get().useRelativeEnergy())
                                   ? Parameters::get().beamEnergy() - part.lastStateVector().energy()
                                   : part.lastStateVector().energy();

    const double first_s = part.firstS();

    if (beamline_->elements().size() < 2) {
      H_WARNING << "Insufficiant number of beamline elements for propagation: " << beamline_->elements().size();
      return;
    }
    try {
      for (auto it = beamline_->begin() + 1; it != beamline_->end(); ++it) {
        // extract the previous and the current element in the beamline
        const auto prev_elem = *(it - 1), elem = *it;
        if (elem->s() > s_max)
          break;

        const Particle::Position in_pos(*part.rbegin());

        // initialise the outwards position
        Particle::Position out_pos(-1., StateVector());

        // between two elements
        if (first_s > prev_elem->s() && first_s < elem->s()) {
          switch (prev_elem->type()) {
            case element::aDrift:
              H_INFO << "Path starts inside drift " << prev_elem->name() << ".";
              break;
            default:
              H_INFO << "Path starts inside element " << prev_elem->name() << ".";
              break;
          }

          // build a temporary element mimicking the drift effect
          auto elem_tmp = prev_elem->clone();
          elem_tmp->setS(first_s);
          elem_tmp->setLength(elem->s() - first_s);
          out_pos = propagateThrough(in_pos, elem_tmp, energy_loss, part.charge());
        }
        // before one element
        if (first_s <= elem->s())
          out_pos = propagateThrough(in_pos, elem, energy_loss, part.charge());

        if (out_pos.s() < 0.)
          continue;  // no new point to add to the particle's trajectory

        part.addPosition(out_pos.s(), out_pos.stateVector());

        if (!Parameters::get().computeApertureAcceptance())
          continue;

        const auto& aper = prev_elem->aperture();
        if (!aper || aper->type() != aperture::anInvalidAperture)
          continue;

        const TwoVector pos_prev_elem(part.stateVectorAt(prev_elem->s()).position());
        if (!aper->contains(pos_prev_elem))
          throw ParticleStoppedException(__PRETTY_FUNCTION__, ExceptionType::warning, prev_elem)
              << "Entering at " << pos_prev_elem << ", s = " << prev_elem->s() << " m\n\t"
              << "Aperture centre at " << aper->position() << "\n\t"
              << "Distance to aperture centre: " << hector::TwoVector(aper->position() - pos_prev_elem).norm() * 1.e2
              << " cm.";
        // has passed through the element?
        //std::cout << prev_elem->s()+prev_elem->length() << "\t" << part.stateVectorAt( prev_elem->s()+prev_elem->length() ).position() << std::endl;
        if (!aper->contains(part.stateVectorAt(prev_elem->s() + prev_elem->length()).position()))
          throw ParticleStoppedException(__PRETTY_FUNCTION__, ExceptionType::warning, prev_elem)
              << "Did not pass aperture " << aper->type() << ".";
      }
    } catch (const ParticleStoppedException&) {
      throw;
    } catch (const Exception&) {
      throw;
    }

    // finish by sorting all positions according to their s-coordinate
    //part.sortPositions();
  }

  bool Propagator::stopped(Particle& part, double s_max) const {
    for (auto it = beamline_->begin() + 1; it != beamline_->end(); ++it) {
      // extract the previous and the current element in the beamline
      const auto prev_elem = *(it - 1), elem = *it;
      if (s_max > 0 && elem->s() > s_max)
        return false;

      const auto& aper = prev_elem->aperture();
      if (aper && aper->type() != aperture::anInvalidAperture) {
        // has passed the element entrance?
        if (!aper->contains(part.stateVectorAt(prev_elem->s()).position()))
          return true;
        // has passed through the element?
        if (!aper->contains(part.stateVectorAt(prev_elem->s() + prev_elem->length()).position()))
          return true;
      }
    }
    return false;
  }

  Particle::Position Propagator::propagateThrough(const Particle::Position& ini_pos,
                                                  const element::ElementPtr& elem,
                                                  double eloss,
                                                  int qp) const {
    try {
      //const StateVector shift( elem->relativePosition(), elem->angles(), 0., 0. );
      //const StateVector shift( elem->relativePosition(), TwoVector(), 0., 0. );
      const StateVector shift(TwoVector(), TwoVector(), 0., 0.);
      const Vector prop =
          elem->matrix(eloss, ini_pos.stateVector().m(), qp) * (ini_pos.stateVector().vector() - shift.vector()) +
          shift.vector();

      if (Parameters::get().loggingThreshold() >= ExceptionType::debug)
        H_DEBUG << "Propagating particle of mass " << ini_pos.stateVector().m() << " GeV"
                << " and state vector at s = " << ini_pos.s() << " m:" << ini_pos.stateVector().vector().transpose()
                << "\t"
                << "through " << elem->type() << " element \"" << elem->name() << "\" "
                << "at s = " << elem->s() << " m, "
                << "of length " << elem->length() << " m,\n\t"
                << "and with transfer matrix:" << elem->matrix(eloss, ini_pos.stateVector().m(), qp) << "\t"
                << "Resulting state vector:" << prop.transpose();

      // perform the propagation (assuming that mass is conserved...)
      StateVector vec(prop, ini_pos.stateVector().m());

      // convert the angles -> tan-1( angle )
      //const TwoVector ang_old = vec.angles();
      //vec.setAngles( math::atan2( ang_old ) );

      return Particle::Position(elem->s() + elem->length(), vec);
    } catch (const Exception& e) {
      throw;
    }
  }

  void Propagator::propagate(Particles& beam, double s_max) const {
    for (auto& part : beam)
      propagate(part, s_max);
  }
}  // namespace hector
