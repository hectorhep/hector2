#include "io/MADXParser.h"

#include "Canvas.h"

#include "TGraph.h"
#include "TMultiGraph.h"

void
drawBothGraphs( const char* name, const char* title, const char* axes, TGraph* gr_x, TGraph* gr_y )
{
  TMultiGraph mg;
  mg.Add( gr_x );
  mg.Add( gr_y );
  gr_x->SetLineColor( kBlack );
  gr_y->SetLineColor( kRed );
  gr_x->SetLineWidth( 2 );
  gr_y->SetLineWidth( 2 );

  Hector::Canvas c( name, title );
  mg.Draw( "al" );
  mg.SetTitle( axes );
  c.Prettify( mg.GetHistogram() );
  c.SetLegendX1( 0.65 );
  c.AddLegendEntry( gr_x, gr_x->GetTitle(), "l" );
  c.AddLegendEntry( gr_y, gr_y->GetTitle(), "l" );
  c.SetGrid();
  c.Save( "pdf" );
}

int
main( int argc, char* argv[] )
{
  if ( argc<2 ) {
    std::cerr << "Usage: " << argv[0] << " <MAD-X output>" << std::endl;
    exit( 0 );
  }

  Hector::Parser::MADX madx( argv[1], "IP5", +1, 440., false );
  const Hector::Beamline* beamline = madx.beamline();

  TGraph gr_betax, gr_betay,
         gr_dispx, gr_dispy,
         gr_relx, gr_rely;

  for ( Hector::Beamline::ElementsMap::const_iterator e=beamline->begin(); e!=beamline->end(); ++e ) {
    const Hector::Element::ElementBase* elem = *e;
    if ( elem->type()==Hector::Element::aDrift ) continue;
    std::cout << elem << "::" << elem->dispersion().x() << std::endl;
    gr_betax.SetPoint( gr_betax.GetN(), elem->s(), elem->beta().x() );
    gr_betay.SetPoint( gr_betay.GetN(), elem->s(), elem->beta().y() );
    gr_dispx.SetPoint( gr_dispx.GetN(), elem->s(), elem->dispersion().x() );
    gr_dispy.SetPoint( gr_dispy.GetN(), elem->s(), elem->dispersion().y() );
    gr_relx.SetPoint( gr_relx.GetN(), elem->s(), elem->relativePosition().x() );
    gr_rely.SetPoint( gr_rely.GetN(), elem->s(), elem->relativePosition().y() );
  }
  gr_betax.SetTitle( "#beta_{X}" );
  gr_betay.SetTitle( "#beta_{Y}" );
  gr_dispx.SetTitle( "D_{X}" );
  gr_dispy.SetTitle( "D_{Y}" );
  gr_relx.SetTitle( "relative X" );
  gr_rely.SetTitle( "relative Y" );

  drawBothGraphs( "beta", "Beta-s dependence", "s (m)\\#beta (m)", &gr_betax, &gr_betay );
  drawBothGraphs( "disp", "Dispersion-s dependence", "s (m)\\D (m)", &gr_dispx, &gr_dispy );
  drawBothGraphs( "relat_align", "Relative alignment-s dependence", "s (m)\\Relative distance (m)", &gr_relx, &gr_rely );

  return 0;
}
