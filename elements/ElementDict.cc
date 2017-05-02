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
    apertype_map_.insert( std::pair<std::string,Aperture::Type>( "none", Aperture::anInvalidType ) );
    apertype_map_.insert( std::pair<std::string,Aperture::Type>( "rectangle", Aperture::aRectangularAperture ) );
    apertype_map_.insert( std::pair<std::string,Aperture::Type>( "ellipse", Aperture::anEllipticAperture ) );
    apertype_map_.insert( std::pair<std::string,Aperture::Type>( "circle", Aperture::aCircularAperture ) );
    apertype_map_.insert( std::pair<std::string,Aperture::Type>( "rectellipse", Aperture::aRectEllipticAperture ) );

    // then fill the elements type map
    elemtype_map_.insert( std::pair<std::string,Element::Type>( "marker", Element::aMarker ) );
    elemtype_map_.insert( std::pair<std::string,Element::Type>( "drift", Element::aDrift ) );
    elemtype_map_.insert( std::pair<std::string,Element::Type>( "monitor", Element::aMonitor ) );
    elemtype_map_.insert( std::pair<std::string,Element::Type>( "quadrupole", Element::aGenericQuadrupole ) );
    elemtype_map_.insert( std::pair<std::string,Element::Type>( "sextupole", Element::aSextupole ) );
    elemtype_map_.insert( std::pair<std::string,Element::Type>( "multipole", Element::aMultipole ) );
    elemtype_map_.insert( std::pair<std::string,Element::Type>( "sbend", Element::aSectorDipole ) );
    elemtype_map_.insert( std::pair<std::string,Element::Type>( "rbend", Element::aRectangularDipole ) );
    elemtype_map_.insert( std::pair<std::string,Element::Type>( "hkicker", Element::anHorizontalKicker ) );
    elemtype_map_.insert( std::pair<std::string,Element::Type>( "vkicker", Element::aVerticalKicker ) );
    elemtype_map_.insert( std::pair<std::string,Element::Type>( "rcollimator", Element::aRectangularCollimator ) );
    elemtype_map_.insert( std::pair<std::string,Element::Type>( "ecollimator", Element::anEllipticalCollimator ) );
    elemtype_map_.insert( std::pair<std::string,Element::Type>( "ccollimator", Element::aCircularCollimator ) );
    elemtype_map_.insert( std::pair<std::string,Element::Type>( "placeholder", Element::aPlaceholder ) );
    elemtype_map_.insert( std::pair<std::string,Element::Type>( "instrument", Element::anInstrument ) );
    elemtype_map_.insert( std::pair<std::string,Element::Type>( "solenoid", Element::aSolenoid ) );
  }
}
