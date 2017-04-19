#ifndef Hector_test_utils_h
#define Hector_test_utils_h

#include "elements/ElementBase.h"

#include "TColor.h"

Color_t
elementColour( const Hector::Element::ElementBase* elem )
{
  switch ( elem->type() ) {
    case Hector::Element::aMarker: return kBlue+3;
    case Hector::Element::aDrift: return kBlack;
    case Hector::Element::aRectangularDipole: return kRed;
    case Hector::Element::aSectorDipole: return kGreen;
    case Hector::Element::aGenericQuadrupole: {
      return ( elem->magneticStrength()>0. )
        ? kYellow // horizontal quadrupole
        : kBlue;  // vertical quadrupole
    }
    case Hector::Element::anHorizontalQuadrupole: return kYellow;
    case Hector::Element::aVerticalQuadrupole: return kBlue;
    case Hector::Element::aVerticalKicker: return kMagenta;
    case Hector::Element::anHorizontalKicker: return kCyan;
    case Hector::Element::aRectangularCollimator: return 8;
    case Hector::Element::anEllipticalCollimator: return 9;
    case Hector::Element::aCircularCollimator: return 10;
    default: return kWhite;
  }
}

#include "TPave.h"
#include "TLatex.h"
#include "TMarker.h"

static const float alpha = 0.8;

void
drawBeamline( const char axis, const Hector::Beamline* bl, const unsigned short beam, const char* ip="IP5", float scale=0.2, float min_s=-999., float max_s=999. )
{
  const float size_y = scale/4., // general scale of the element x/y position
              scale_y = 0.8, // element x/y displacement magnification factor
              offset_at_s = 120.;

  TLatex txt;
  txt.SetTextFont( 130+2 );

  for ( Hector::Elements::const_iterator it=bl->begin(); it!=bl->end(); it++ ) {
    const Hector::Element::ElementBase* elem = *it;
    if ( elem->type()==Hector::Element::aDrift ) continue; //FIXME
    if ( min_s!=-999. and elem->s()<min_s ) continue;
    if ( max_s!=+999. and elem->s()>max_s ) continue;
    //if ( elem->type()==Hector::Element::ElementBase::Marker and elem->name()!=ip ) continue;

    // introduce a x- and y-offset for drawing purposes
    int offset = 0;
    /*if ( elem->s()>offset_at_s ) {
      if ( beam==0 ) offset = -1;
      if ( beam==1 ) offset = +1;
    }*/
    const float pos_rel = ( axis=='x' ) ? elem->x() : elem->y(),
                pos_x_ini = elem->s(),
                pos_x_end = pos_x_ini + elem->length(),
                pos_y_low = ( ( beam==0 ) ? -1 : 0 ) * ( size_y ) + pos_rel*scale_y /*+ offset*size_y*/,
                pos_y_high = pos_y_low+size_y;

    // ROOT and its brilliant memory management...
    TPave* elem_box = new TPave( pos_x_ini, pos_y_low, pos_x_end, pos_y_high, 1 );
    elem_box->SetLineColor( kGray );
    //elem_box->SetFillStyle( 1001 );
    elem_box->SetFillColorAlpha( elementColour( elem ), alpha );
    elem_box->Draw();

    if ( elem->type()!=Hector::Element::aMarker or elem->name()!=ip ) {
      txt.SetTextSize( 0.015 );
      txt.SetTextAngle( 90. );
      txt.SetTextAlign( 22 );
      txt.DrawLatex( elem->s()+elem->length()/2., 3.* ( ( beam==0 ) ? -1 : +1 ) * ( size_y ), elem->name().c_str() );
    }
    else {
      txt.SetTextSize( 0.035 );
      txt.SetTextAlign( 12 );
      txt.SetTextAngle( 45. );
      txt.DrawLatex( pos_x_ini, 0., elem->name().c_str() );
    }

  }
  /*for ( Hector::Beamline::MarkersMap::const_iterator it=bl->markers_begin(); it!=bl->markers_end(); it++ ) {
    txt.SetTextSize( 0.01 );
    txt.SetTextAngle( 90. );
    txt.DrawLatex( it->first, -2*size_y, it->second.name().c_str() );
  }*/
}

#include "TLegend.h"
#include <map>
#include "Canvas.h"

class elementsLegend : public TLegend
{
  public:
    elementsLegend( const Hector::Beamline* bl=0, float xmin=0.07, float ymin=0.07, float xmax=0.93, float ymax=0.93 ) :
      TLegend( xmin, ymin, xmax, ymax )
    {
      if ( bl ) feedBeamline( bl );
      TLegend::SetTextFont( font_type( 2 ) );
      TLegend::SetHeader( "Elements legend" );
    }
    ~elementsLegend()
    {
      for ( std::map<Hector::Element::Type, TPave*>::iterator it=already_in_.begin(); it!=already_in_.end(); it++ ) {
        if ( it->second ) delete ( it->second );
      }
      already_in_.clear();
    }

    void feedBeamline( const Hector::Beamline* bl )
    {
      for ( Hector::Elements::const_iterator it=bl->begin(); it!=bl->end(); it++ ) {
        const Hector::Element::Type type = ( *it )->type();
        // skip the markers and drifts
        if ( type==Hector::Element::aMarker ) continue;
        if ( type==Hector::Element::aDrift ) continue;
        // skip the elements already added
        if ( already_in_.find( type )!=already_in_.end() ) continue;

        TPave* pv = new TPave( 0., 0., 0., 0., 0 );
        pv->SetFillColorAlpha( elementColour( ( *it ) ), alpha );
        pv->SetLineColor( kGray );
        pv->SetLineWidth( 4 );
        std::ostringstream os; os << type;
        TLegend::AddEntry( pv, os.str().c_str(), "f" );
        already_in_.insert( std::pair<Hector::Element::Type, TPave*>( type, pv ) );
      }
      if ( already_in_.size()>7 ) TLegend::SetNColumns( 2 );

    }
  private:
    std::map<Hector::Element::Type, TPave*> already_in_;
};

#endif
