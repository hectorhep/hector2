#include "ElementDict.h"

namespace Element
{
  bool Dictionary::built_ = false;
  Dictionary* Dictionary::dictionary_ = 0;

  Dictionary*
  Dictionary::get()
  {
    if ( !built_ ) {
      dictionary_ = new Dictionary;
      built_ = true;
    }
    return dictionary_;
  }

  Dictionary::Dictionary()
  {
    // first fill the aperture types map
    apertype_map_.insert( std::pair<std::string,ApertureBase::Type>( "none", ApertureBase::None ) );
    apertype_map_.insert( std::pair<std::string,ApertureBase::Type>( "rectangle", ApertureBase::Rectangular ) );
    apertype_map_.insert( std::pair<std::string,ApertureBase::Type>( "ellipse", ApertureBase::Elliptic ) );
    apertype_map_.insert( std::pair<std::string,ApertureBase::Type>( "circle", ApertureBase::Circular ) );
    apertype_map_.insert( std::pair<std::string,ApertureBase::Type>( "rectellipse", ApertureBase::RectElliptic ) );

    // then fill the elements type map
    elemtype_map_.insert( std::pair<std::string,ElementBase::Type>( "marker", ElementBase::Marker ) );
    elemtype_map_.insert( std::pair<std::string,ElementBase::Type>( "drift", ElementBase::Drift ) );
    elemtype_map_.insert( std::pair<std::string,ElementBase::Type>( "monitor", ElementBase::Monitor ) );
    elemtype_map_.insert( std::pair<std::string,ElementBase::Type>( "quadrupole", ElementBase::Quadrupole ) );
    elemtype_map_.insert( std::pair<std::string,ElementBase::Type>( "sextupole", ElementBase::Sextupole ) );
    elemtype_map_.insert( std::pair<std::string,ElementBase::Type>( "multipole", ElementBase::Multipole ) );
    elemtype_map_.insert( std::pair<std::string,ElementBase::Type>( "sbend", ElementBase::SectorDipole ) );
    elemtype_map_.insert( std::pair<std::string,ElementBase::Type>( "rbend", ElementBase::RectangularDipole ) );
    elemtype_map_.insert( std::pair<std::string,ElementBase::Type>( "hkicker", ElementBase::HorizontalKicker ) );
    elemtype_map_.insert( std::pair<std::string,ElementBase::Type>( "vkicker", ElementBase::VerticalKicker ) );
    elemtype_map_.insert( std::pair<std::string,ElementBase::Type>( "placeholder", ElementBase::Placeholder ) );
    elemtype_map_.insert( std::pair<std::string,ElementBase::Type>( "instrument", ElementBase::Instrument ) );
  }
}
