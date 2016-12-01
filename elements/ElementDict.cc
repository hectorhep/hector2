#include "ElementDict.h"

namespace Hector
{
  ElementDictionary&
  ElementDictionary::get()
  {
    static ElementDictionary instance;
    return instance;
  }

  ElementDictionary::ElementDictionary()
  {
    // first fill the aperture types map
    apertype_map_.insert( std::pair<std::string,Aperture::ApertureBase::Type>( "none", Aperture::ApertureBase::None ) );
    apertype_map_.insert( std::pair<std::string,Aperture::ApertureBase::Type>( "rectangle", Aperture::ApertureBase::Rectangular ) );
    apertype_map_.insert( std::pair<std::string,Aperture::ApertureBase::Type>( "ellipse", Aperture::ApertureBase::Elliptic ) );
    apertype_map_.insert( std::pair<std::string,Aperture::ApertureBase::Type>( "circle", Aperture::ApertureBase::Circular ) );
    apertype_map_.insert( std::pair<std::string,Aperture::ApertureBase::Type>( "rectellipse", Aperture::ApertureBase::RectElliptic ) );

    // then fill the elements type map
    elemtype_map_.insert( std::pair<std::string,Element::ElementBase::Type>( "marker", Element::ElementBase::Marker ) );
    elemtype_map_.insert( std::pair<std::string,Element::ElementBase::Type>( "drift", Element::ElementBase::Drift ) );
    elemtype_map_.insert( std::pair<std::string,Element::ElementBase::Type>( "monitor", Element::ElementBase::Monitor ) );
    elemtype_map_.insert( std::pair<std::string,Element::ElementBase::Type>( "quadrupole", Element::ElementBase::Quadrupole ) );
    elemtype_map_.insert( std::pair<std::string,Element::ElementBase::Type>( "sextupole", Element::ElementBase::Sextupole ) );
    elemtype_map_.insert( std::pair<std::string,Element::ElementBase::Type>( "multipole", Element::ElementBase::Multipole ) );
    elemtype_map_.insert( std::pair<std::string,Element::ElementBase::Type>( "sbend", Element::ElementBase::SectorDipole ) );
    elemtype_map_.insert( std::pair<std::string,Element::ElementBase::Type>( "rbend", Element::ElementBase::RectangularDipole ) );
    elemtype_map_.insert( std::pair<std::string,Element::ElementBase::Type>( "hkicker", Element::ElementBase::HorizontalKicker ) );
    elemtype_map_.insert( std::pair<std::string,Element::ElementBase::Type>( "vkicker", Element::ElementBase::VerticalKicker ) );
    elemtype_map_.insert( std::pair<std::string,Element::ElementBase::Type>( "rcollimator", Element::ElementBase::RectangularCollimator ) );
    elemtype_map_.insert( std::pair<std::string,Element::ElementBase::Type>( "ecollimator", Element::ElementBase::EllipticalCollimator ) );
    elemtype_map_.insert( std::pair<std::string,Element::ElementBase::Type>( "ccollimator", Element::ElementBase::CircularCollimator ) );
    elemtype_map_.insert( std::pair<std::string,Element::ElementBase::Type>( "placeholder", Element::ElementBase::Placeholder ) );
    elemtype_map_.insert( std::pair<std::string,Element::ElementBase::Type>( "instrument", Element::ElementBase::Instrument ) );
  }
}
