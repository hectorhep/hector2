#include "Hector/Apertures/Circular.h"
#include "Hector/Apertures/Elliptic.h"
#include "Hector/Apertures/Rectangular.h"
#include "Hector/Apertures/RectElliptic.h"

#include "TGraph.h"
#include "TMultiGraph.h"
#include "Canvas.h"

#include <iomanip>
#include <memory>

int main() {
  const float x_min = -0.5, x_max = 0.5, y_min = -0.5, y_max = 0.5;
  const unsigned short num_steps = 500;

  hector::aperture::Rectangular aper_rect(0.35, 0.25);
  hector::aperture::Elliptic aper_el(0.4, 0.2);
  hector::aperture::RectElliptic aper(0.35, 0.25, 0.4, 0.2, CLHEP::Hep2Vector(0.0, 0.0));
  CLHEP::Hep2Vector limits = aper.limits();

  std::cout << "Aperture has limits: " << limits.x() << " / " << limits.y() << std::endl;

  TGraph g_inside, g_outside, g_elliptic, g_rect;
  for (unsigned short i = 0; i < num_steps; i++) {
    const float x = x_min + (x_max - x_min) * i / (num_steps - 1);
    for (unsigned short j = 0; j < num_steps; j++) {
      const float y = y_min + (y_max - y_min) * j / (num_steps - 1);
      const CLHEP::Hep2Vector pos(x, y);
      if (aper_rect.contains(pos))
        g_rect.SetPoint(g_rect.GetN(), x, y);
      if (aper_el.contains(pos))
        g_elliptic.SetPoint(g_elliptic.GetN(), x, y);
      if (aper.contains(pos))
        g_inside.SetPoint(g_inside.GetN(), x, y);
      /*else {
        g_outside.SetPoint( g_outside.GetN(), x, y );
      }*/
    }
  }
  hector::Canvas c("aperture_plot", "Rect-elliptic aperture (0.35, 0.25, 0.4, 0.2)");
  TMultiGraph mg;
  g_inside.SetMarkerStyle(20);
  g_inside.SetMarkerColor(kRed + 1);
  g_inside.SetFillColor(kRed + 1);
  g_inside.SetLineWidth(1);
  g_inside.SetMarkerSize(0.5);
  g_rect.SetMarkerStyle(20);
  g_rect.SetMarkerColorAlpha(kGray, 0.5);
  g_rect.SetFillColorAlpha(kGray, 0.5);
  g_rect.SetLineWidth(1);
  g_rect.SetMarkerSize(0.5);
  g_elliptic.SetMarkerStyle(20);
  g_elliptic.SetMarkerColorAlpha(kBlue + 3, 0.5);
  g_elliptic.SetFillColorAlpha(kBlue + 3, 0.5);
  g_elliptic.SetLineWidth(1);
  g_elliptic.SetMarkerSize(0.5);
  /*g_outside.SetMarkerStyle( 20 );
  g_outside.SetMarkerColor( kGray );
  g_outside.SetMarkerSize( 0.5 );*/
  //mg.Add( &g_outside );
  mg.Add(&g_elliptic);
  mg.Add(&g_rect);
  mg.Add(&g_inside);
  c.SetLegendX1(0.45);
  c.AddLegendEntry(&g_elliptic, "Elliptic component", "f");
  c.AddLegendEntry(&g_rect, "Rectangular component", "f");
  c.AddLegendEntry(&g_inside, "Rect-ellipse", "f");

  mg.Draw("ap");
  mg.GetXaxis()->SetLimits(-0.5, 0.5);
  mg.GetYaxis()->SetRangeUser(-0.4, 0.5);
  mg.GetHistogram()->SetTitle("x (arb. unit)@@y (arb. units)");
  c.Prettify(mg.GetHistogram());
  c.Save("pdf");
  return 0;
}
