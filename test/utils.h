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

#ifndef Hector_test_utils_h
#define Hector_test_utils_h

#include <TArrow.h>
#include <TColor.h>
#include <TGraph.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TMarker.h>
#include <TPave.h>

#include <map>

#include "Canvas.h"
#include "Hector/Elements/ElementFwd.h"

void fillElementStyle(const hector::element::ElementPtr& elem, Color_t& col, Style_t& style) {
  col = kWhite;
  style = 1001;
  switch (elem->type()) {
    case hector::element::aMarker:
      col = kBlue + 3;
      break;
    case hector::element::aDrift:
      col = kBlack;
      break;
    case hector::element::aRectangularDipole:
      col = kRed;
      style = 3004;
      break;
    case hector::element::aSectorDipole:
      col = kGreen;
      style = 3005;
      break;
    case hector::element::aGenericQuadrupole: {
      col = (elem->magneticStrength() > 0.) ? kYellow  // horizontal quadrupole
                                            : kBlue;   // vertical quadrupole
    } break;
    case hector::element::anHorizontalQuadrupole:
      col = kYellow;
      style = 3001;
      break;
    case hector::element::aVerticalQuadrupole:
      col = kBlue;
      style = 3002;
      break;
    case hector::element::aVerticalKicker:
      col = kMagenta;
      break;
    case hector::element::anHorizontalKicker:
      col = kCyan;
      break;
    case hector::element::aRectangularCollimator:
      col = 8;
      style = 3006;
      break;
    case hector::element::anEllipticalCollimator:
      col = 9;
      style = 3007;
      break;
    case hector::element::aCircularCollimator:
      col = 10;
      break;
    default:
      col = kWhite;
      break;
  }
}

//static const float alpha = 0.8;
static const float alpha = 1.0;

void drawBeamline(const char axis,
                  const hector::Beamline* bl,
                  const unsigned short beam,
                  const char* ip = "IP5",
                  float scale = 0.2,
                  float min_s = -999.,
                  float max_s = 999.,
                  bool draw_apertures = false) {
  const float size_y = scale / 4.,  // general scale of the element x/y position
      scale_y = 0.8;                // element x/y displacement magnification factor
                                    //offset_at_s = 120.;

  TLatex txt;
  txt.SetTextFont(130 + 2);
  auto g_aper = new TGraph;
  const float aper_arr_len = 0.999;

  for (const auto& elemPtr : *bl) {
    if (elemPtr->type() == hector::element::aDrift)
      continue;  //FIXME
    if (min_s != -999. && elemPtr->s() < min_s)
      continue;
    if (max_s != +999. && elemPtr->s() > max_s)
      break;
    //if ( elemPtr->type() == hector::element::ElementBase::Marker && elemPtr->name() != ip ) continue;

    // introduce a x- and y-offset for drawing purposes
    /*int offset = 0;
    if ( elemPtr->s() > offset_at_s ) {
      if ( beam == 0 ) offset = -1;
      if ( beam == 1 ) offset = +1;
    }*/
    const float pos_rel = (axis == 'x') ? elemPtr->x() : elemPtr->y(), pos_x_ini = elemPtr->s(),
                pos_x_end = pos_x_ini + elemPtr->length(),
                pos_y_low = ((beam == 1) ? -1 : 0) * size_y + pos_rel * scale_y /*+ offset*size_y*/,
                pos_y_high = pos_y_low + size_y;
    if (draw_apertures) {
      const auto& aper = elemPtr->aperture();
      if (aper) {
        const auto& aper_lim = aper->limits();
        const float pos_aper = (axis == 'x') ? aper->x() : aper->y(),
                    half_len_aper = (axis == 'x') ? aper_lim.x() : aper_lim.y();
        const float min_pos = pos_rel + pos_aper - half_len_aper, max_pos = pos_rel + pos_aper + half_len_aper;
        auto arr1 = new TArrow(elemPtr->s(), max_pos * aper_arr_len, elemPtr->s(), max_pos, 0.01, "-|"),
             arr2 = new TArrow(elemPtr->s(), min_pos, elemPtr->s(), min_pos * aper_arr_len, 0.01, "|-");
        arr1->SetLineColor(kGray + 2);
        arr2->SetLineColor(kGray + 2);
        if (fabs(pos_aper + half_len_aper) < scale)
          arr1->Draw();
        if (fabs(pos_aper - half_len_aper) < scale)
          arr2->Draw();
        g_aper->SetPoint(g_aper->GetN(), elemPtr->s(), (axis == 'x') ? aper_lim.x() : aper_lim.y());
      }
    }

    // ROOT and its brilliant memory management...
    auto elem_box = new TPave(pos_x_ini, pos_y_low, pos_x_end, pos_y_high, 1);
    elem_box->SetLineColor(kGray + 1);
    //elem_box->SetFillStyle( 1001 );
    short fill_colour, fill_style;
    fillElementStyle(elemPtr, fill_colour, fill_style);
    //elem_box->SetFillColorAlpha( elementColour( elemPtr ), alpha );
    elem_box->SetFillColor(fill_colour);
    elem_box->SetFillStyle(fill_style);
    elem_box->Draw();

    if (elemPtr->type() != hector::element::aMarker || elemPtr->name() != ip) {
      txt.SetTextSize(0.012);
      txt.SetTextAngle(90.);
      txt.SetTextAlign(22);
      txt.DrawLatex(
          elemPtr->s() + elemPtr->length() / 2., 3.2 * ((beam == 0) ? -1 : +1) * size_y, elemPtr->name().c_str());
    } else {
      txt.SetTextSize(0.035);
      txt.SetTextAlign(12);
      txt.SetTextAngle(45.);
      txt.DrawLatex(pos_x_ini, 0., elemPtr->name().c_str());
    }
  }
  /*if ( draw_apertures ) {
    g_aper->Draw( "l" );
    g_aper->SetLineColor( kGray );
  }*/
  /*for ( hector::Beamline::MarkersMap::const_iterator it=bl->markers_begin(); it!=bl->markers_end(); it++ ) {
    txt.SetTextSize( 0.01 );
    txt.SetTextAngle( 90. );
    txt.DrawLatex( it->first, -2*size_y, it->second.name().c_str() );
  }*/
}

/// Helper class to build a beamline legend in ROOT
class elementsLegend : public TLegend {
public:
  /// Build a helper producer with attributes
  /// \params[in] bl Hector beamline to parse
  elementsLegend(
      const hector::Beamline* bl = nullptr, float xmin = 0.05, float ymin = 0.05, float xmax = 0.95, float ymax = 0.95)
      : TLegend(xmin, ymin, xmax, ymax) {
    if (bl)
      feedBeamline(bl);
    TLegend::SetTextFont(font_type(2));
    TLegend::SetHeader("#font[32]{Elements legend}");
  }

  void feedBeamline(const hector::Beamline* bl) {
    for (const auto& elemPtr : *bl) {
      const hector::element::Type type = elemPtr->type();
      // skip the markers and drifts
      if (type == hector::element::aMarker)
        continue;
      if (type == hector::element::anInstrument)
        continue;
      if (type == hector::element::aMonitor)
        continue;
      if (type == hector::element::aDrift)
        continue;
      // skip the elements already added
      if (already_in_.count(type) > 0)
        continue;

      auto pv = new TPave(0., 0., 0., 0., 0);
      short fill_colour, fill_style;
      fillElementStyle(elemPtr, fill_colour, fill_style);
      pv->SetFillColorAlpha(fill_colour, alpha);
      pv->SetFillStyle(fill_style);
      pv->SetLineColor(kGray);
      pv->SetLineWidth(4);
      TLegend::AddEntry(pv, elemPtr->typeName().c_str(), "f");
      already_in_[type].reset(pv);
    }
    if (already_in_.size() > 7)
      TLegend::SetNColumns(2);
  }

private:
  std::map<hector::element::Type, std::unique_ptr<TPave> > already_in_;
};

#endif
