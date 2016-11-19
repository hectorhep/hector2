#include "elements/ElementBase.h"

#include "TColor.h"

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

