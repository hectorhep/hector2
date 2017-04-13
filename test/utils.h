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

void
drawBeamline( const char axis, const Hector::Beamline* bl, const unsigned short beam, float size_x, const char* ip )
{
  const float size_y = 0.05, // general scale of the element x/y position
              scale_y = 3.; // element x/y displacement magnification factor

  TLatex txt;
  txt.SetTextSize( 0.035 );
  txt.SetTextFont( 132 );
  txt.SetTextAlign( 22 );
  txt.SetTextAngle( 45. );

  for ( Hector::Beamline::ElementsMap::const_iterator it=bl->begin(); it!=bl->end(); it++ ) {
    Hector::Element::ElementBase* elem = *it;
    if ( elem->type()==Hector::Element::aDrift ) continue; //FIXME
    //if ( elem->type()==Hector::Element::ElementBase::Marker and elem->name()!=ip ) continue;

    // introduce a x- and y-offset for drawing purposes
    int offset = 0;
    if ( elem->s()>120. ) {
      if ( beam==0 ) offset = +1;
      if ( beam==1 ) offset = -1;
    }
    const float pos_x_ini = elem->s(),
                pos_x_end = pos_x_ini + elem->length(),
                pos_y_low = ( ( axis=='x' ) ? elem->x() : elem->y() )*scale_y-size_y/2.+offset*size_y,
                pos_y_high = pos_y_low+size_y;

    if ( elem->type()==Hector::Element::aMarker and elem->name()==ip ) {
      TMarker* arr = new TMarker( pos_x_ini, 0., 20 ),
              *arr2 = dynamic_cast<TMarker*>( arr->Clone() );
      arr->SetMarkerColor( kWhite );
      arr->SetMarkerSize( 2. );
      arr2->SetMarkerSize( 2. );
      arr2->SetMarkerStyle( 4 );
      arr->Draw();
      arr2->Draw();
      txt.DrawLatex( pos_x_ini, 0., elem->name().c_str() );
    }
    /*if ( elem->type()==Hector::Element::ElementBase::Marker ) {
g      txt.DrawLatex( elem->s(), pos_y_low, elem->name().c_str() );
    }*/

    // ROOT and its brilliant memory management...
    TPave* elem_box = new TPave( pos_x_ini, pos_y_low, pos_x_end, pos_y_high, 0 );
    //elem_box->SetFillStyle( 1001 );
    elem_box->SetFillColorAlpha( elementColour( elem ), 1.0 );
    elem_box->Draw();
  }
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
      for ( Hector::Beamline::ElementsMap::const_iterator it=bl->begin(); it!=bl->end(); it++ ) {
        const Hector::Element::Type type = ( *it )->type();
        // skip the markers and drifts
        if ( type==Hector::Element::aMarker ) continue;
        if ( type==Hector::Element::aDrift ) continue;
        // skip the elements already added
        if ( already_in_.find( type )!=already_in_.end() ) continue;

        TPave* pv = new TPave( 0., 0., 0., 0., 0 );
        pv->SetFillColorAlpha( elementColour( ( *it ) ), 1.0 );
        pv->SetLineColor( kWhite );
        std::ostringstream os; os << type;
        TLegend::AddEntry( pv, os.str().c_str(), "f" );
        already_in_.insert( std::pair<Hector::Element::Type, TPave*>( type, pv ) );
      }
      if ( already_in_.size()>7 ) TLegend::SetNColumns( 2 );

    }
  private:
    std::map<Hector::Element::Type, TPave*> already_in_;
};
