#include "Hector/IO/HBLFileHandler.h"
#include "Hector/Core/Exception.h"

#include "Hector/Beamline/Beamline.h"

#include "Hector/Elements/Drift.h"
#include "Hector/Elements/Kicker.h"
#include "Hector/Elements/Dipole.h"
#include "Hector/Elements/Quadrupole.h"
#include "Hector/Elements/RectangularCollimator.h"
#include "Hector/Elements/RectangularAperture.h"
#include "Hector/Elements/CircularAperture.h"
#include "Hector/Elements/EllipticAperture.h"
#include "Hector/Elements/RectEllipticAperture.h"

#include "Hector/IO/HBLFileStructures.h"

#include <fstream>
#include <sstream>

namespace Hector
{
  namespace IO
  {
    HBL::HBL( const char* filename ) :
      beamline_( new Beamline )
    {
      parse( filename );
    }

    HBL::HBL( HBL& rhs ) :
      beamline_( std::move( rhs.beamline_ ) )
    {}

    void
    HBL::parse( const char* filename )
    {
      std::ifstream file( filename, std::ios::binary | std::ios::in );
      if ( !file.is_open() )
        throw Exception( __PRETTY_FUNCTION__, Form( "Impossible to open file \"%s\" for reading!", filename ), Fatal );

      HBLHeader hdr;
      file.read( reinterpret_cast<char*>( &hdr ), sizeof( HBLHeader ) );
      if ( hdr.magic != magic_number )
        throw Exception( __PRETTY_FUNCTION__, Form( "Invalid magic number retrieved for file \"%s\"!", filename ), Fatal );

      if ( hdr.version > version )
        throw Exception( __PRETTY_FUNCTION__, Form( "Version %d is not (yet) supported! Currently peaking at %d!", hdr.version, version ), Fatal );

      HBLElement el;
      std::shared_ptr<Element::ElementBase> elem;
      while ( file.read( reinterpret_cast<char*>( &el ), sizeof( HBLElement ) ) ) {
        if ( Parameters::get()->loggingThreshold() > JustWarning ) {
          std::ostringstream os; os << ( Element::Type )el.element_type;
          Exception( __PRETTY_FUNCTION__, Form( "Retrieved a %s element\n\t"
                                                "with name %s\n\tat s=%f m\n\t"
                                                "with length=%f m (magnetic strength=%f)",
                                                os.str().c_str(), el.element_name, el.element_s, el.element_length, el.element_magnetic_strength ), Info ).dump();
        }
        switch ( ( Element::Type )el.element_type ) {
          case Element::aMarker:
          case Element::aMonitor:
            elem = std::make_shared<Element::Marker>( el.element_name, el.element_s, el.element_length );
            break;
          case Element::aDrift:
            elem = std::make_shared<Element::Drift>( el.element_name, el.element_s, el.element_length );
            break;
          case Element::aRectangularDipole:
            elem = std::make_shared<Element::RectangularDipole>( el.element_name, el.element_s, el.element_length, el.element_magnetic_strength );
            break;
          case Element::aSectorDipole:
            elem = std::make_shared<Element::SectorDipole>( el.element_name, el.element_s, el.element_length, el.element_magnetic_strength );
            break;
          //case Element::aGenericQuadrupole:
          case Element::aVerticalQuadrupole:
            elem = std::make_shared<Element::VerticalQuadrupole>( el.element_name, el.element_s, el.element_length, el.element_magnetic_strength );
            break;
          case Element::anHorizontalQuadrupole:
            elem = std::make_shared<Element::HorizontalQuadrupole>( el.element_name, el.element_s, el.element_length, el.element_magnetic_strength );
            break;
          case Element::aVerticalKicker:
            elem = std::make_shared<Element::VerticalKicker>( el.element_name, el.element_s, el.element_length, el.element_magnetic_strength );
            break;
          case Element::anHorizontalKicker:
            elem = std::make_shared<Element::HorizontalKicker>( el.element_name, el.element_s, el.element_length, el.element_magnetic_strength );
            break;
          case Element::aRectangularCollimator:
            elem = std::make_shared<Element::RectangularCollimator>( el.element_name, el.element_s, el.element_length );
            break;
          //case Element::anEllipticalCollimator:
          //case Element::aCircularCollimator:
          default:
            throw Exception( __PRETTY_FUNCTION__, Form( "Invalid element type: %s", (int)el.element_type ), Fatal );
        }
        switch ( ( Aperture::Type )el.aperture_type ) {
          case Aperture::anInvalidAperture: break;
          case Aperture::aRectangularAperture:
            elem->setAperture( std::make_shared<Aperture::RectangularAperture>( el.aperture_p1, el.aperture_p2, TwoVector( el.aperture_x, el.aperture_y ) ) );
            break;
          case Aperture::anEllipticAperture:
            elem->setAperture( std::make_shared<Aperture::EllipticAperture>( el.aperture_p1, el.aperture_p2, TwoVector( el.aperture_x, el.aperture_y ) ) );
            break;
          case Aperture::aCircularAperture:
            elem->setAperture( std::make_shared<Aperture::CircularAperture>( el.aperture_p1, TwoVector( el.aperture_x, el.aperture_y ) ) );
            break;
          case Aperture::aRectEllipticAperture:
            elem->setAperture( std::make_shared<Aperture::RectEllipticAperture>( el.aperture_p1, el.aperture_p2, el.aperture_p3, el.aperture_p4, TwoVector( el.aperture_x, el.aperture_y ) ) );
            break;
          case Aperture::aRectCircularAperture:
            elem->setAperture( std::make_shared<Aperture::RectEllipticAperture>( el.aperture_p1, el.aperture_p2, el.aperture_p3, el.aperture_p3, TwoVector( el.aperture_x, el.aperture_y ) ) );
            break;
          //case Aperture::aRaceTrackAperture:
          //case Aperture::anOctagonalAperture:
          default:
            throw Exception( __PRETTY_FUNCTION__, Form( "Invalid aperture type: %s", (int)el.aperture_type ), Fatal );
        }
        if ( elem ) beamline_->add( elem );
      }
      if ( beamline_->numElements() != hdr.num_elements )
        throw Exception( __PRETTY_FUNCTION__, Form( "Expecting %d elements, retrieved %d!", hdr.num_elements, beamline_->numElements() ), Fatal );
    }

    void
    HBL::write( const Beamline* bl, const char* filename )
    {
      std::ofstream file( filename, std::ios::binary | std::ios::out );
      { // start by writing the file header
        HBLHeader hdr;
        hdr.magic = magic_number;
        hdr.version = version;
        hdr.num_elements = bl->numElements();
        file.write( reinterpret_cast<char*>( &hdr ), sizeof( HBLHeader ) );
      }
      { // then add all individual elements
        for ( const auto& elem : *bl ) {
          HBLElement el;
          el.element_type = elem->type();
          if ( !elem->name().empty() )
            elem->name().copy( el.element_name, sizeof( el.element_name ) );
          el.element_s = elem->s();
          el.element_length = elem->length();
          el.element_magnetic_strength = elem->magneticStrength();
          if ( elem->aperture() ) {
            el.aperture_type = elem->aperture()->type();
            if ( elem->aperture()->parameters().size() > 0 ) el.aperture_p1 = elem->aperture()->p( 0 );
            if ( elem->aperture()->parameters().size() > 1 ) el.aperture_p2 = elem->aperture()->p( 1 );
            if ( elem->aperture()->parameters().size() > 2 ) el.aperture_p3 = elem->aperture()->p( 2 );
            if ( elem->aperture()->parameters().size() > 3 ) el.aperture_p4 = elem->aperture()->p( 3 );
            el.aperture_x = elem->aperture()->x();
            el.aperture_y = elem->aperture()->y();
          }
          file.write( reinterpret_cast<char*>( &el ), sizeof( HBLElement ) );
        }
      }
    }
  }
}
