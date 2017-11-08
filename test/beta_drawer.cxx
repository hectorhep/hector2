#include "Hector/IO/MADXParser.h"

#include "Canvas.h"

#include "TGraph.h"
#include "TMultiGraph.h"
#include "TLatex.h"

void
drawBothGraphs( const char* name, const char* title, const char* axes, TGraph* gr_x, TGraph* gr_y, std::vector<TLatex*> labels, TPave* rp_region, float max_s )
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
  mg.GetXaxis()->SetRangeUser( -max_s, max_s );

  for ( unsigned short i=0; i<labels.size(); i++ ) {
    //std::cout << labels[i] << std::endl;
    TLatex* lab = dynamic_cast<TLatex*>( labels[i]->Clone() );
    //lab->SetY( mg.GetHistogram()->GetMaximum()/2. );
    lab->SetTextAlign( ( i%2 ) ? 32 : 12 );
    lab->SetY( ( i%2 ? -1 : 1 ) * mg.GetHistogram()->GetMaximum()/30. );
    lab->Draw( "same" );
  }
  rp_region->SetY1( mg.GetHistogram()->GetMinimum() );
  rp_region->SetY2( mg.GetHistogram()->GetMaximum() );
  rp_region->Draw( "same" );

  c.Prettify( mg.GetHistogram() );
  c.SetLegendX1( 0.25 );
  c.AddLegendEntry( gr_x, gr_x->GetTitle(), "l" );
  c.AddLegendEntry( gr_y, gr_y->GetTitle(), "l" );
  c.SetGrid();
  c.Save( "pdf" );
}

int
main( int argc, char* argv[] )
{
  if ( argc<2 ) {
    std::cerr << "Usage: " << argv[0] << " <MAD-X output> [maximum s]" << std::endl;
    exit( 0 );
  }

  const float max_s = ( argc>2 ) ? atof( argv[2] ) : 500.;

  Hector::Parser::MADX madx( argv[1], "IP5", +1, max_s );
  const Hector::Beamline* beamline = madx.rawBeamline();

  TGraph gr_betax, gr_betay,
         gr_dispx, gr_dispy,
         gr_relx, gr_rely;

  std::regex rgx_cmspipe( "CMSPIPE[0-9]{1,2}\\.[L,R][0-9]" ),
             rgx_rp( "XRP[V,H]\\.[A-Za-z][0-9][A-Za-z][0-9]\\.B[1,2]" );

  TLatex txt;
  txt.SetTextFont( 130+2 );
  txt.SetTextAngle( 90 );
  txt.SetTextSize( 0.015 );
  txt.SetTextAlign( 12 );

  std::vector<TLatex*> labels;

  float min_rp = 999., max_rp = 0.;

  for ( const auto& elemPtr : *beamline ) {
    if ( elemPtr->type() == Hector::Element::aDrift ) continue;
    //std::cout << elemPtr << "::" << elemPtr->dispersion().x() << std::endl;
    if ( fabs( elemPtr->s() )>max_s && fabs( elemPtr->s()+elemPtr->length() ) > max_s ) continue;
    if ( elemPtr->type() == Hector::Element::aMarker && !std::regex_match( elemPtr->name(), rgx_cmspipe ) ) {
      //std::cout << ">>> " << elemPtr->name() << std::endl;
      labels.push_back( txt.DrawLatex( elemPtr->s(), 100., elemPtr->name().c_str() ) );
    }
    if ( elemPtr->type() == Hector::Element::aRectangularCollimator && std::regex_match( elemPtr->name(), rgx_rp ) ) {
      if ( fabs( elemPtr->s() ) < fabs( min_rp ) ) min_rp = elemPtr->s();
      if ( fabs( elemPtr->s()+elemPtr->length() ) > fabs( max_rp ) ) max_rp = elemPtr->s();
    }

    gr_betax.SetPoint( gr_betax.GetN(), elemPtr->s(), elemPtr->beta().x() );
    gr_betay.SetPoint( gr_betay.GetN(), elemPtr->s(), elemPtr->beta().y() );
    gr_dispx.SetPoint( gr_dispx.GetN(), elemPtr->s(), elemPtr->dispersion().x() );
    gr_dispy.SetPoint( gr_dispy.GetN(), elemPtr->s(), elemPtr->dispersion().y() );
    gr_relx.SetPoint( gr_relx.GetN(), elemPtr->s(), elemPtr->relativePosition().x() );
    gr_rely.SetPoint( gr_rely.GetN(), elemPtr->s(), elemPtr->relativePosition().y() );
  }
  gr_betax.SetTitle( "#beta_{X}" );
  gr_betay.SetTitle( "#beta_{Y}" );
  gr_dispx.SetTitle( "D_{X}" );
  gr_dispy.SetTitle( "D_{Y}" );
  gr_relx.SetTitle( "relative X" );
  gr_rely.SetTitle( "relative Y" );

  TPave* rp_region = new TPave( min_rp, 0., max_rp, 1., 3 );
  rp_region->SetFillColorAlpha( kGray, 0.5 );
  rp_region->SetLineColor( kBlack );

  drawBothGraphs( "beta", "Beta-s dependence", "s (m)\\#beta (m)", &gr_betax, &gr_betay, labels, rp_region, max_s );
  drawBothGraphs( "disp", "Dispersion-s dependence", "s (m)\\D (m)", &gr_dispx, &gr_dispy, labels, rp_region, max_s );
  drawBothGraphs( "relat_align", "Relative alignment-s dependence", "s (m)\\Relative distance (m)", &gr_relx, &gr_rely, labels, rp_region, max_s );

  return 0;
}
