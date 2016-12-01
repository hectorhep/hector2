#include "elements/ElementBase.h"

#include "TColor.h"
#include "TPave.h"

using namespace Hector;

Color_t
elementColour( const Element::ElementBase* elem )
{
  switch ( elem->type() ) {
    case Element::ElementBase::Marker: return kWhite;
    case Element::ElementBase::Drift: return kBlack;
    case Element::ElementBase::RectangularDipole: return kRed;
    case Element::ElementBase::SectorDipole: return kGreen;
    case Element::ElementBase::Quadrupole: {
      if ( elem->magneticStrength()>0. ) return kYellow; // horizontal quadrupole
      else return kBlue; // vertical quadrupole
    }
    case Element::ElementBase::VerticalKicker: return kMagenta;
    case Element::ElementBase::HorizontalKicker: return kCyan;
    case Element::ElementBase::RectangularCollimator: return 8;
    case Element::ElementBase::EllipticalCollimator: return 9;
    case Element::ElementBase::CircularCollimator: return 10;
    default: return kWhite;
  }
}


void
drawBeamline( const char axis, const Beamline* bl, const unsigned short beam, float size_x, const char* ip )
{
  const float size_y = 0.05,
              scale_y = 3.;

  for ( Beamline::ElementsMap::const_iterator it=bl->begin(); it!=bl->end(); it++ ) {
    Element::ElementBase* elem = *it;
    if ( elem->type()==Element::ElementBase::Drift ) continue; //FIXME
    if ( elem->type()==Element::ElementBase::Marker and elem->name()!=ip ) continue;

    // introduce a x- and y-offset for drawing purposes
    int offset = 0;
    if ( elem->s()>120. ) {
      if ( beam==0 ) offset = +1;
      if ( beam==1 ) offset = -1;
    }
    /*const float pos_x_ini = elem->s()/bl->length(),
                pos_x_end = pos_x_ini + elem->length()/bl->length(),*/
    const float pos_x_ini = elem->s(),
                pos_x_end = pos_x_ini + elem->length(),
                pos_y_low = ( ( axis=='x' ) ? elem->x()*scale_y-size_y/2.+offset*size_y : elem->y()*scale_y-size_y/2.+offset*size_y ),
//                pos_y_low_x = elem->relativePosition().x()*scale_y-size_y/2.,
                pos_y_high = pos_y_low+size_y;

    // ROOT and its brilliant memory management...
    TPave* elem_box = new TPave( pos_x_ini, pos_y_low, pos_x_end, pos_y_high, 0 );
    //elem_box->SetFillStyle( 1001 );
    elem_box->SetFillColorAlpha( elementColour( elem ), 0.3 );
    elem_box->Draw();
  }
}
