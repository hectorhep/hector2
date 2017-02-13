#include "elements/ElementBase.h"

#include "TColor.h"
#include "TPave.h"
#include "TLatex.h"
#include "TMarker.h"

Color_t
elementColour( const Hector::Element::ElementBase* elem )
{
  switch ( elem->type() ) {
    case Hector::Element::ElementBase::Marker: return kBlue+3;
    case Hector::Element::ElementBase::Drift: return kBlack;
    case Hector::Element::ElementBase::RectangularDipole: return kRed;
    case Hector::Element::ElementBase::SectorDipole: return kGreen;
    case Hector::Element::ElementBase::Quadrupole: {
      if ( elem->magneticStrength()>0. ) return kYellow; // horizontal quadrupole
      else return kBlue; // vertical quadrupole
    }
    case Hector::Element::ElementBase::VerticalKicker: return kMagenta;
    case Hector::Element::ElementBase::HorizontalKicker: return kCyan;
    case Hector::Element::ElementBase::RectangularCollimator: return 8;
    case Hector::Element::ElementBase::EllipticalCollimator: return 9;
    case Hector::Element::ElementBase::CircularCollimator: return 10;
    default: return kWhite;
  }
}


void
drawBeamline( const char axis, const Hector::Beamline* bl, const unsigned short beam, float size_x, const char* ip )
{
  const float size_y = 0.05, // general scale of the element x/y position
              scale_y = 3.; // element x/y displacement magnification factor

  TLatex txt;
  txt.SetTextSize( 0.03 );
  txt.SetTextAngle( 45. );

  for ( Hector::Beamline::ElementsMap::const_iterator it=bl->begin(); it!=bl->end(); it++ ) {
    Hector::Element::ElementBase* elem = *it;
    if ( elem->type()==Hector::Element::ElementBase::Drift ) continue; //FIXME
    //if ( elem->type()==Hector::Element::ElementBase::Marker and elem->name()!=ip ) continue;

    // introduce a x- and y-offset for drawing purposes
    int offset = 0;
    if ( elem->s()>120. ) {
      if ( beam==0 ) offset = +1;
      if ( beam==1 ) offset = -1;
    }
    const float pos_x_ini = elem->s(),
                pos_x_end = pos_x_ini + elem->length(),
                pos_y_low = ( ( axis=='x' ) ? elem->x()*scale_y-size_y/2.+offset*size_y : elem->y()*scale_y-size_y/2.+offset*size_y ),
                pos_y_high = pos_y_low+size_y;

    if ( elem->type()==Hector::Element::ElementBase::Marker and elem->name()==ip ) {
      TMarker* arr = new TMarker( pos_x_ini, 0., 24 );
      arr->Draw();
    }
    /*if ( elem->type()==Hector::Element::ElementBase::Marker ) {
      txt.DrawLatex( elem->s(), pos_y_low, elem->name().c_str() );
    }*/

    // ROOT and its brilliant memory management...
    TPave* elem_box = new TPave( pos_x_ini, pos_y_low, pos_x_end, pos_y_high, 0 );
    //elem_box->SetFillStyle( 1001 );
    elem_box->SetFillColorAlpha( elementColour( elem ), 0.3 );
    elem_box->Draw();
  }
}
