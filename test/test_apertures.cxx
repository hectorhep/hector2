#include "elements/CircularAperture.h"
#include "elements/EllipticAperture.h"
#include "elements/RectangularAperture.h"
#include "elements/RectEllipticAperture.h"

#include "TGraph.h"
#include "TMultiGraph.h"
#include "Canvas.h"

#include <sstream>
#include <iomanip>

int
main()
{
  const float x_min = -0.5, x_max = 0.5,
              y_min = -0.5, y_max = 0.5;
  const unsigned short num_steps = 100;

  Hector::Aperture::ApertureBase* aper;

  //aper = new Hector::Aperture::CircularAperture( 0.5, CLHEP::Hep2Vector( 0.1, 0.3 ) );
  //aper = new Hector::Aperture::RectangularAperture( 0.5, 0.1, CLHEP::Hep2Vector( 0.1, 0.3 ) );
  aper = new Hector::Aperture::RectEllipticAperture( 0.2, 0.3, 0.25, 0.4, CLHEP::Hep2Vector( 0.0, 0.0 ) );
  //aper = new Hector::Aperture::RectEllipticAperture( 0.064, 0.0265, 0.064, 0.0265, CLHEP::Hep2Vector( 0.0, 0.0 ) );
  float lim_x, lim_y;
  aper->limits( lim_x, lim_y );
  
  std::ostringstream os; os << "Aperture: " << std::setprecision( 2 ) << aper;
  std::cout << os.str() << " has limits: " << lim_x << " / " << lim_y << std::endl;

  TGraph g_inside, g_outside;
  for ( unsigned short i=0; i<num_steps; i++ ) {
    const float x = x_min + ( x_max-x_min ) * i / num_steps;
    for ( unsigned short j=0; j<num_steps; j++ ) {
      const float y = y_min + ( y_max-y_min ) * j / num_steps;
      const CLHEP::Hep2Vector pos( x, y );
      if ( aper->contains( pos ) ) {
        g_inside.SetPoint( g_inside.GetN(), x, y );
      }
      else {
        g_outside.SetPoint( g_outside.GetN(), x, y );
      }
    }
  }
  Hector::Canvas c( "aperture_plot", os.str().c_str() );
  TMultiGraph mg;
  g_inside.SetMarkerStyle( 20 );
  g_inside.SetMarkerColor( kRed );
  g_inside.SetMarkerSize( 0.5 );
  g_outside.SetMarkerStyle( 20 );
  g_outside.SetMarkerColor( kGray );
  g_outside.SetMarkerSize( 0.5 );
  mg.Add( &g_inside );
  mg.Add( &g_outside );
  mg.Draw( "ap" );
  c.Prettify( mg.GetHistogram() );
  c.Save( "png" );
  return 0;
}
