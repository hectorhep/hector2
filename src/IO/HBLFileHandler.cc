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

#include <fstream>
#include <sstream>

#include "Hector/Apertures/Circular.h"
#include "Hector/Apertures/Elliptic.h"
#include "Hector/Apertures/RectElliptic.h"
#include "Hector/Apertures/Rectangular.h"
#include "Hector/Beamline.h"
#include "Hector/Elements/Collimator.h"
#include "Hector/Elements/Dipole.h"
#include "Hector/Elements/Drift.h"
#include "Hector/Elements/Kicker.h"
#include "Hector/Elements/Quadrupole.h"
#include "Hector/Exception.h"
#include "Hector/IO/HBLFileHandler.h"
#include "Hector/IO/HBLFileStructures.h"
#include "Hector/Parameters.h"

namespace hector {
  namespace io {
    HBL::HBL(const std::string& filename) : beamline_(new Beamline) { parse(filename); }

    HBL::HBL(HBL& rhs) : beamline_(std::move(rhs.beamline_)) {}

    void HBL::parse(const std::string& filename) {
      std::ifstream file(filename, std::ios::binary | std::ios::in);
      if (!file.is_open())
        throw H_ERROR << "Impossible to open file \"" << filename << "\" for reading!";

      HBLHeader hdr;
      file.read(reinterpret_cast<char*>(&hdr), sizeof(HBLHeader));
      if (hdr.magic != magic_number)
        throw H_ERROR << "Invalid magic number retrieved for file \"" << filename << "\"!";

      if (hdr.version > version)
        throw H_ERROR << "Version " << hdr.version << " is not (yet) supported! Currently peaking at " << version
                      << "!";

      HBLElement el;
      element::ElementPtr elem;
      while (file.read(reinterpret_cast<char*>(&el), sizeof(HBLElement))) {
        if (Parameters::get().loggingThreshold() > ExceptionType::warning)
          H_INFO << "Retrieved a " << (element::Type)el.element_type << " element\n\t"
                 << "with name " << el.element_name << "\n\tat s=" << el.element_s << " m\n\t"
                 << "with length=" << el.element_length << " m (magnetic strength=" << el.element_magnetic_strength
                 << ").";
        switch ((element::Type)el.element_type) {
          case element::aMarker:
          case element::aMonitor:
          case element::anInstrument:
            elem = std::make_shared<element::Marker>(el.element_name, el.element_s, el.element_length);
            break;
          case element::aDrift:
            elem = std::make_shared<element::Drift>(el.element_name, el.element_s, el.element_length);
            break;
          case element::aRectangularDipole:
            elem = std::make_shared<element::RectangularDipole>(
                el.element_name, el.element_s, el.element_length, el.element_magnetic_strength);
            break;
          case element::aSectorDipole:
            elem = std::make_shared<element::SectorDipole>(
                el.element_name, el.element_s, el.element_length, el.element_magnetic_strength);
            break;
          //case element::aGenericQuadrupole:
          case element::aVerticalQuadrupole:
            elem = std::make_shared<element::VerticalQuadrupole>(
                el.element_name, el.element_s, el.element_length, el.element_magnetic_strength);
            break;
          case element::anHorizontalQuadrupole:
            elem = std::make_shared<element::HorizontalQuadrupole>(
                el.element_name, el.element_s, el.element_length, el.element_magnetic_strength);
            break;
          case element::aVerticalKicker:
            elem = std::make_shared<element::VerticalKicker>(
                el.element_name, el.element_s, el.element_length, el.element_magnetic_strength);
            break;
          case element::anHorizontalKicker:
            elem = std::make_shared<element::HorizontalKicker>(
                el.element_name, el.element_s, el.element_length, el.element_magnetic_strength);
            break;
          case element::aRectangularCollimator:
          case element::anEllipticalCollimator:
          case element::aCircularCollimator:
          case element::aCollimator:
            elem = std::make_shared<element::Collimator>(el.element_name, el.element_s, el.element_length);
            break;
          //case element::anEllipticalCollimator:
          //case element::aCircularCollimator:
          default:
            throw H_ERROR << "Invalid element type: " << (int)el.element_type << ".";
        }
        switch ((aperture::Type)el.aperture_type) {
          case aperture::anInvalidAperture:
            break;
          case aperture::aRectangularAperture:
            elem->setAperture(std::make_shared<aperture::Rectangular>(
                el.aperture_p1, el.aperture_p2, TwoVector(el.aperture_x, el.aperture_y)));
            break;
          case aperture::anEllipticAperture:
            elem->setAperture(std::make_shared<aperture::Elliptic>(
                el.aperture_p1, el.aperture_p2, TwoVector(el.aperture_x, el.aperture_y)));
            break;
          case aperture::aCircularAperture:
            elem->setAperture(
                std::make_shared<aperture::Circular>(el.aperture_p1, TwoVector(el.aperture_x, el.aperture_y)));
            break;
          case aperture::aRectEllipticAperture:
            elem->setAperture(std::make_shared<aperture::RectElliptic>(el.aperture_p1,
                                                                       el.aperture_p2,
                                                                       el.aperture_p3,
                                                                       el.aperture_p4,
                                                                       TwoVector(el.aperture_x, el.aperture_y)));
            break;
          case aperture::aRectCircularAperture:
            elem->setAperture(std::make_shared<aperture::RectElliptic>(el.aperture_p1,
                                                                       el.aperture_p2,
                                                                       el.aperture_p3,
                                                                       el.aperture_p3,
                                                                       TwoVector(el.aperture_x, el.aperture_y)));
            break;
          //case aperture::aRaceTrackAperture:
          //case aperture::anOctagonalAperture:
          default:
            throw H_ERROR << "Invalid aperture type: " << (int)el.aperture_type << ".";
        }
        if (elem)
          beamline_->add(elem);
      }
      if (beamline_->numElements() != hdr.num_elements)
        throw H_ERROR << "Expecting " << hdr.num_elements << " elements, retrieved " << beamline_->numElements() << "!";
    }

    void HBL::write(const Beamline* bl, const std::string& filename) {
      std::ofstream file(filename, std::ios::binary | std::ios::out);
      {  // start by writing the file header
        HBLHeader hdr;
        hdr.magic = magic_number;
        hdr.version = version;
        hdr.num_elements = bl->numElements();
        file.write(reinterpret_cast<char*>(&hdr), sizeof(HBLHeader));
      }
      {  // then add all individual elements
        for (const auto& elem : *bl) {
          HBLElement el;
          el.element_type = elem->type();
          if (!elem->name().empty())
            elem->name().copy(el.element_name, sizeof(el.element_name));
          el.element_s = elem->s();
          el.element_length = elem->length();
          el.element_magnetic_strength = elem->magneticStrength();
          if (elem->aperture()) {
            el.aperture_type = elem->aperture()->type();
            if (elem->aperture()->parameters().size() > 0)
              el.aperture_p1 = elem->aperture()->p(0);
            if (elem->aperture()->parameters().size() > 1)
              el.aperture_p2 = elem->aperture()->p(1);
            if (elem->aperture()->parameters().size() > 2)
              el.aperture_p3 = elem->aperture()->p(2);
            if (elem->aperture()->parameters().size() > 3)
              el.aperture_p4 = elem->aperture()->p(3);
            el.aperture_x = elem->aperture()->x();
            el.aperture_y = elem->aperture()->y();
          }
          file.write(reinterpret_cast<char*>(&el), sizeof(HBLElement));
        }
      }
    }
  }  // namespace io
}  // namespace hector
