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

#ifndef HectorAddOns_ROOT_DrawUtils_h
#define HectorAddOns_ROOT_DrawUtils_h

#include <TColor.h>
#include <TLegend.h>
#include <TPave.h>

#include <map>

#include "Canvas.h"
#include "Hector/Elements/ElementFwd.h"

void fillElementStyle(const hector::element::ElementPtr& elem, Color_t& col, Style_t& style);

//static const float alpha = 0.8;
static const float alpha = 1.0;

void drawBeamline(const char axis,
                  const hector::Beamline* bl,
                  const unsigned short beam,
                  const char* ip = "IP5",
                  float scale = 0.2,
                  float min_s = -999.,
                  float max_s = 999.,
                  bool draw_apertures = false);

/// Helper class to build a beamline legend in ROOT
class elementsLegend : public TLegend {
public:
  /// Build a helper producer with attributes
  /// \params[in] bl Hector beamline to parse
  explicit elementsLegend(
      const hector::Beamline* bl = nullptr, float xmin = 0.05, float ymin = 0.05, float xmax = 0.95, float ymax = 0.95);

  void feedBeamline(const hector::Beamline* bl);

private:
  std::map<hector::element::Type, std::unique_ptr<TPave> > already_in_;
};

#endif
