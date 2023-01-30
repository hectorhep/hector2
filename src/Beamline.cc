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

#include <algorithm>
#include <iostream>
#include <regex>
#include <sstream>

#include "Hector/Beamline.h"
#include "Hector/Elements/Drift.h"
#include "Hector/Exception.h"
#include "Hector/Particle.h"
#include "Hector/Utils/String.h"

namespace hector {
  Beamline::Beamline() : max_length_(0.) {}

  Beamline::Beamline(const Beamline& rhs, bool copy_elements)
      : max_length_(rhs.max_length_), ip_(rhs.ip_), markers_(rhs.markers_) {
    clear();
    if (copy_elements)
      setElements(rhs);
  }

  Beamline::Beamline(double length, const element::ElementPtr& ip)
      : max_length_(length + 5.),  // artificially increase the size to include next elements
        ip_(ip) {}

  Beamline::~Beamline() {
    clear();
    markers_.clear();
  }

  void Beamline::clear() { elements_.clear(); }

  void Beamline::addMarker(const element::Marker& marker) {
    markers_.insert(std::pair<double, element::Marker>(marker.s(), marker));
  }

  void Beamline::add(const element::ElementPtr& elem) {
    const double new_size = elem->s() + elem->length();
    if (new_size > max_length_ && max_length_ < 0.)
      throw H_ERROR << "Element " << elem->name() << " is too far away for this beamline!\n"
                    << "\tBeamline length: " << max_length_ << " m, this element: " << new_size << " m.";

    bool already_added = false;

    // check the overlaps before adding
    for (auto& prev_elem : elements_) {
      // first check if the element is already present in the beamline
      if (prev_elem->name() == elem->name()) {
        already_added = true;
        break;
      }

      if (prev_elem->s() > elem->s())
        break;
      if (prev_elem->s() + prev_elem->length() <= elem->s())
        continue;
      if (prev_elem->length() == 0)
        continue;
      if (prev_elem->s() == elem->s() && elem->length() == 0)
        continue;

      if (!Parameters::get().correctBeamlineOverlaps())
        throw H_ERROR << "Elements overlap with \"" << prev_elem->name() << "\" "
                      << "detected while adding \"" << elem->name() << "\"!";

      // from that point on, an overlap is detected
      // reduce or separate that element in two sub-parts

      H_DEBUG << elem->name() << " (" << elem->type() << ") is inside " << prev_elem->name() << " ("
              << prev_elem->type() << ")\n\t"
              << "Hector will fix the overlap by splitting the earlier.";
      const double prev_length = prev_elem->length();

      element::ElementPtr next_elem = nullptr;
      // check if one needs to add an extra piece to the previous element
      if (elem->s() + elem->length() < prev_elem->s() + prev_elem->length()) {
        const std::string prev_name = prev_elem->name();
        prev_elem->setName(format("%s/1", prev_name.c_str()));
        next_elem = prev_elem->clone();
        next_elem->setName(format("%s/2", prev_name.c_str()));
        next_elem->setS(elem->s() + elem->length());
        next_elem->setLength(prev_length - elem->length());
        next_elem->setBeta(elem->beta());
        next_elem->setDispersion(elem->dispersion());
        next_elem->setRelativePosition(elem->relativePosition());
        next_elem->setParentElement(prev_elem);
      }

      prev_elem->setLength(elem->s() - prev_elem->s());

      elements_.push_back(elem);
      already_added = true;

      if (next_elem != nullptr)
        elements_.push_back(next_elem);

      break;
    }

    if (!already_added)
      elements_.push_back(elem);

    // sort all beamline elements according to their s-position
    std::sort(elements_.begin(), elements_.end(), element::ElementsSorter());
  }

  const element::ElementPtr& Beamline::get(const std::string& name) const {
    for (const auto& elem : elements_)
      if (elem->name().find(name) != std::string::npos)
        return elem;
    H_WARNING << "Beamline element \"" << name << "\" not found.";
    return *elements_.end();
  }

  element::ElementPtr& Beamline::get(const std::string& name) {
    for (auto& elem : elements_)
      if (elem->name().find(name) != std::string::npos)
        return elem;
    H_WARNING << "Beamline element \"" << name << "\" not found.";
    return *elements_.end();
  }

  const element::ElementPtr& Beamline::get(double s) const {
    for (const auto& elem : elements_) {
      if (elem->s() > s)
        continue;
      if (elem->s() <= s && elem->s() + elem->length() >= s)
        return elem;
    }
    H_WARNING << "Beamline has no element at s=" << s << ".";
    return *elements_.end();
  }

  element::ElementPtr& Beamline::get(double s) {
    for (auto& elem : elements_) {
      if (elem->s() > s)
        continue;
      if (elem->s() <= s && elem->s() + elem->length() >= s)
        return elem;
    }
    return *elements_.end();
  }

  element::Elements Beamline::find(const std::string& regex) {
    try {
      std::regex rgx_search(regex);
      std::cmatch m;
      element::Elements out;
      for (auto& elem : elements_)
        if (std::regex_search(elem->name().c_str(), m, rgx_search))
          out.emplace_back(elem);
      return out;
    } catch (const std::regex_error& e) {
      throw H_ERROR << "Invalid regular expression required:\n\t" << regex << "\n\tError code: " << e.code() << ".";
    }
  }

  Matrix Beamline::matrix(double eloss, double mp, int qp) const {
    Matrix out = DiagonalMatrix(6, 1);

    for (const auto& elem : elements_) {
      const auto& mat = elem->matrix(eloss, mp, qp);
      H_INFO << "Multiplication by transfer matrix of element \"" << elem->name() << "\".\n"
             << " value: " << mat;
      out = out * mat;
    }

    return out;
  }

  double Beamline::length() const {
    if (elements_.empty())
      return 0.;
    const auto& elem = *elements_.rbegin();
    return elem->s() + elem->length();
  }

  void Beamline::dump(std::ostream& os, bool show_drifts) const {
    os << "=============== Beamline dump ===============\n";
    if (ip_)
      os << " interaction point: " << ip_->name() << " at " << ip_->position() << "\n";
    os << " length: " << length() << " m\n"
       << " elements list: ";
    for (const auto& elemPtr : elements_) {
      if (!show_drifts && elemPtr->type() == element::aDrift)
        continue;
      os << "\n  * " << *elemPtr;
    }
    os << "\n";
  }

  void Beamline::offsetElementsAfter(double s, const TwoVector& offset) {
    for (auto& elemPtr : elements_) {
      if (elemPtr->s() < s)
        continue;
      elemPtr->offset(offset);
    }
  }

  void Beamline::tiltElementsAfter(double s, const TwoVector& tilt) {
    for (auto& elemPtr : elements_) {
      if (elemPtr->s() < s)
        continue;
      elemPtr->tilt(tilt);
    }
  }

  std::unique_ptr<Beamline> Beamline::sequencedBeamline(const Beamline* beamline) {
    // add the drifts between optical elements
    double pos = 0.;
    // brand new beamline to populate
    std::unique_ptr<Beamline> tmp(new Beamline(*beamline, false));

    // convert all empty spaces into drifts
    for (const auto& elemPtr : *beamline) {
      // skip the markers
      if (elemPtr->type() == element::aMarker && elemPtr->name() != beamline->interactionPoint()->name())
        continue;
      // add a drift whenever there is a gap in s
      const double drift_length = elemPtr->s() - pos;
      try {
        if (drift_length > 0.)
          tmp->add(std::make_shared<element::Drift>(format("drift:%.4E", pos).c_str(), pos, drift_length));
        tmp->add(elemPtr);
      } catch (const Exception& e) {
        e.dump(std::cerr);
      }
      pos = elemPtr->s() + elemPtr->length();
    }
    // add the last drift
    const double drift_length = tmp->length() - pos;
    if (drift_length > 0) {
      try {
        tmp->add(std::make_shared<element::Drift>(format("drift:%.4E", pos).c_str(), pos, drift_length));
      } catch (const Exception& e) {
        e.dump(std::cerr);
      }
    }

    return tmp;
  }

  void Beamline::setElements(const Beamline& moth_bl) {
    for (const auto& elem : moth_bl)
      add(elem);
  }
}  // namespace hector
