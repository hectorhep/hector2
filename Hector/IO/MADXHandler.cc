#include "Hector/IO/MADXHandler.h"

#include "Hector/Core/Exception.h"

#include "Hector/Beamline/Beamline.h"

#include "Hector/Elements/Quadrupole.h"
#include "Hector/Elements/Dipole.h"
#include "Hector/Elements/RectangularCollimator.h"
#include "Hector/Elements/Kicker.h"
#include "Hector/Elements/Marker.h"

#include "Hector/Elements/EllipticAperture.h"
#include "Hector/Elements/CircularAperture.h"
#include "Hector/Elements/RectangularAperture.h"
#include "Hector/Elements/RectEllipticAperture.h"

namespace Hector
{
/*  namespace ParametersMap
  {
    //template class Unordered<IO::MADX::ValueType>;
    template const std::string Unordered<IO::MADX::ValueType>::key( const size_t i ) const;
  }*/
  namespace IO
  {
    std::regex MADX::rgx_typ_( "^\\%[0-9]{0,}(s|le)$" );
    std::regex MADX::rgx_hdr_( "^\\@ (\\w+) +\\%([0-9]+s|le) +\\\"?([^\"\\n]+)" );
    std::regex MADX::rgx_elm_hdr_( "^\\s{0,}([\\*\\$])(.+)" );
    std::regex MADX::rgx_rp_vert_name_( "XRPV\\.[0-9a-zA-Z]{4}\\.B[1,2]" );
    std::regex MADX::rgx_rp_horiz_name_( "XRPH\\.[0-9a-zA-Z]{4}\\.B[1,2]" );
    std::regex MADX::rgx_drift_name_( "DRIFT\\_[0-9]+" );
    std::regex MADX::rgx_quadrup_name_( "M[B,Q]\\w+\\d?\\.\\w?\\d[L,R]\\d(\\.B[1,2])?" );
    std::regex MADX::rgx_sect_dipole_name_( "MB\\.[A-Z][0-9]{1,2}[L,R][0-9]\\.B[1,2]" );
    std::regex MADX::rgx_rect_dipole_name_( "MB[A-Z0-9]{2,3}\\.*[B,R][0-9]" );
    std::regex MADX::rgx_ip_name_( "IP[0-9]" );
    std::regex MADX::rgx_monitor_name_( "BPM.+" );
    std::regex MADX::rgx_rect_coll_name_( "T[C,A].*\\.\\d[L,R]\\d\\.?(B[1-9])?" );

    MADX::MADX( std::string filename, std::string ip_name, int direction, float max_s ) :
      in_file_( filename ),
      dir_( direction/abs( direction ) ),
      ip_name_( ip_name ), s_offset_( 0. ), found_interaction_point_( false ),
      has_next_element_( false )
    {
      if ( !in_file_.is_open() )
        throw Exception( __PRETTY_FUNCTION__, Form( "Failed to open Twiss file \"%s\"\n\tCheck the path!", filename.c_str() ), Fatal );
      parseHeader();

      raw_beamline_ = std::unique_ptr<Beamline>( new Beamline( max_s ) );
      if ( max_s < 0. && header_float_.hasKey( "length" ) ) raw_beamline_->setLength( header_float_.get( "length" ) );
      if ( header_float_.hasKey( "energy" ) && Parameters::get()->beamEnergy() != header_float_.get( "energy" ) ) {
        Parameters::get()->setBeamEnergy( header_float_.get( "energy" ) );
        PrintWarning( Form( "Beam energy changed to %.1f GeV to match the MAD-X optics parameters", Parameters::get()->beamEnergy() ) );
      }
      if ( header_float_.hasKey( "mass" ) && Parameters::get()->beamParticlesMass() != header_float_.get( "mass" ) ) {
        Parameters::get()->setBeamParticlesMass( header_float_.get( "mass" ) );
        PrintWarning( Form( "Beam particles mass changed to %.4f GeV to match the MAD-X optics parameters", Parameters::get()->beamParticlesMass() ) );
      }
      if ( header_float_.hasKey( "charge" ) && Parameters::get()->beamParticlesCharge() != static_cast<int>( header_float_.get( "charge" ) ) ) {
        Parameters::get()->setBeamParticlesCharge( static_cast<int>( header_float_.get( "charge" ) ) );
        PrintWarning( Form( "Beam particles charge changed to %d e to match the MAD-X optics parameters", Parameters::get()->beamParticlesCharge() ) );
      }

      parseElementsFields();

      // start by identifying the interaction point
      findInteractionPoint();

      // then parse all elements
      parseElements();

      beamline_ = Beamline::sequencedBeamline( raw_beamline_.get() );
    }

    MADX::MADX( const char* filename, const char* ip_name, int direction, float max_s ) :
      MADX( std::string( filename ), std::string( ip_name ), direction, max_s ) {}

    MADX::MADX( const MADX& rhs ) :
      dir_( rhs.dir_ ), ip_name_( rhs.ip_name_ ), s_offset_( rhs.s_offset_ ),
      found_interaction_point_( rhs.found_interaction_point_ ), has_next_element_( rhs.has_next_element_ )
    {}

    MADX::MADX( MADX& rhs ) :
      beamline_( std::move( rhs.beamline_ ) ), raw_beamline_( std::move( rhs.raw_beamline_ ) ),
      dir_( rhs.dir_ ), ip_name_( rhs.ip_name_ ), s_offset_( rhs.s_offset_ ),
      found_interaction_point_( rhs.found_interaction_point_ ), has_next_element_( rhs.has_next_element_ )
    {}

    Beamline*
    MADX::beamline() const
    {
      if ( !beamline_ ) {
        PrintWarning( "Sequenced beamline not computed from the MAD-X Twiss file. Retrieving the raw version. You may encounter some numerical issues." );
        return raw_beamline_.get();
      }
      return beamline_.get();
    }

    void
    MADX::printInfo() const
    {
      std::ostringstream os;
      os << "MAD-X output successfully parsed. Metadata:";
      if ( header_str_.hasKey( "title" ) ) os << "\n\t Title: " << header_str_.get( "title" );
      if ( header_str_.hasKey( "origin" ) ) os << "\n\t Origin: " << trim( header_str_.get( "origin" ) );
      if ( header_str_.hasKey( "date" ) or header_str_.hasKey( "time" ) ) os << "\n\t Export date: " << trim( header_str_.get( "date" ) ) << " @ " << trim( header_str_.get( "time" ) );
      if ( header_float_.hasKey( "energy" ) ) os << "\n\t Simulated single beam energy: " << header_float_.get( "energy" ) << " GeV";
      if ( header_str_.hasKey( "sequence" ) ) os << "\n\t Sequence: " << header_str_.get( "sequence" );
      if ( header_str_.hasKey( "particle" ) ) os << "\n\t Beam particles: " << header_str_.get( "particle" );
      if ( header_float_.hasKey( "length" ) ) os << "\n\t Maximal beamline length: " << header_float_.get( "length" ) << " m";
      PrintInfo( os.str().c_str() );
    }

    Elements
    MADX::romanPots( const RPType& type ) const
    {
      Elements out;
      if ( !raw_beamline_ ) {
        PrintWarning( "Beamline not yet parsed! returning an empty list" );
        return out;
      }
      for ( const auto& elemPtr : *raw_beamline_ ) {
        try {
          if ( std::regex_match( elemPtr->name(), rgx_rp_horiz_name_ ) ) {
            if ( type == allPots || type == horizontalPots ) out.push_back( elemPtr );
          }
          if ( std::regex_match( elemPtr->name(), rgx_rp_vert_name_ ) ) {
            if ( type == allPots || type == verticalPots ) out.push_back( elemPtr );
          }
        } catch ( std::regex_error& e ) {
          throw Exception( __PRETTY_FUNCTION__, Form( "Error while parsing Roman pots!\n\t%s", e.what() ), Fatal );
        }
      }
      return out;
    }

    void
    MADX::parseHeader()
    {
      if ( !in_file_.is_open() )
        throw Exception( __PRETTY_FUNCTION__, "Twiss file is not opened nor ready for parsing!", Fatal );
      std::string line;
      while ( !in_file_.eof() ) {
        std::getline( in_file_, line );

        try {
          std::smatch match;
          if ( !std::regex_search( line, match, rgx_hdr_ ) ) return;

          const std::string key = lowercase( match.str( 1 ) );
          if ( match.str( 2 ) == "le" )
            header_float_.add( key, atof( match.str( 3 ).c_str() ) );
          else
            header_str_.add( key, match.str( 3 ) );

          // keep track of the last line read in the file
          in_file_lastline_ = in_file_.tellg();
        } catch ( std::regex_error& e ) {
          throw Exception( __PRETTY_FUNCTION__, Form( "Error while parsing the header!\n\t%s", e.what() ), Fatal );
        }
      }
    }

    void
    MADX::parseElementsFields()
    {
      if ( !in_file_.is_open() )
        throw Exception( __PRETTY_FUNCTION__, "Twiss file is not opened nor ready for parsing!", Fatal );
      std::string line;

      in_file_.seekg( in_file_lastline_ );
      std::vector<std::string> list_names, list_types;

      while ( !in_file_.eof() ) {
        std::getline( in_file_, line );

        try {
          std::smatch match;
          if ( !std::regex_search( line, match, rgx_elm_hdr_ ) ) break;

          std::string field;
          std::stringstream str( match.str( 2 ) );
          switch ( match.str( 1 )[0] ) {
            case '*': while ( str >> field ) { list_names.push_back( lowercase( field ) ); } break; // field names
            case '$': while ( str >> field ) { list_types.push_back( field ); } break; // field types
            default: break;
          }
          in_file_lastline_ = in_file_.tellg();
        } catch ( std::regex_error& e ) {
          throw Exception( __PRETTY_FUNCTION__, Form( "Error while parsing elements fields!\n\t%s", e.what() ), Fatal );
        }
      }

      // perform the matching name <-> data type
      bool has_lists_matching = ( list_names.size() == list_types.size() );
      for ( unsigned short i = 0; i < list_names.size(); i++ ) {
        ValueType type = Unknown;
        try {
          std::smatch match;
          if ( has_lists_matching && std::regex_search( list_types.at( i ), match, rgx_typ_ ) ) {
            if ( match.str( 1 ) == "le" ) type = Float;
            else if ( match.str( 1 ) == "s" ) type = String;
          }
          elements_fields_.add( list_names.at( i ), type );
        } catch ( std::regex_error& e ) {
          throw Exception( __PRETTY_FUNCTION__, Form( "Error while performing the matching name-data!\n\t%s", e.what() ), Fatal );
        }
      }
    }

    void
    MADX::findInteractionPoint()
    {
      if ( !in_file_.is_open() )
        throw Exception( __PRETTY_FUNCTION__, "Twiss file is not opened nor ready for parsing!", Fatal );
      std::string line;
      in_file_.seekg( in_file_lastline_ );

      while ( !in_file_.eof() ) {
        std::getline( in_file_, line );
        std::stringstream str( trim( line ) );
        if ( str.str().length() == 0 ) continue;
        std::string buffer;
        ValuesCollection values;
        while ( str >> buffer ) { values.push_back( buffer ); }
        // first check if the "correct" number of element properties is parsed
        if ( values.size()!=elements_fields_.size() ) {
          throw Exception( __PRETTY_FUNCTION__, Form( "MAD-X output seems corrupted!\n\t"
                                                      "Element %s has %d fields when %d are expected.",
                                                      trim( values.at( 0 ) ).c_str(), values.size(), elements_fields_.size() ), Fatal );
        }
        try {
          auto elem = parseElement( values );
          if ( !elem ) continue;
          if ( elem->name() == ip_name_ ) {
            found_interaction_point_ = true;
            s_offset_ = elem->s();
            raw_beamline_->addElement( elem );
            return;
          }
        } catch ( Exception& e ) {
          e.dump();
          throw Exception( __PRETTY_FUNCTION__, Form( "Failed to retrieve the interaction point with name=\"%s\"", ip_name_.c_str() ), Fatal );
        }
      }
    }

    void
    MADX::parseElements()
    {
      if ( !in_file_.is_open() )
        throw Exception( __PRETTY_FUNCTION__, "Twiss file is not opened nor ready for parsing!", Fatal );
      // parse the optics elements and their characteristics
      std::string line;

      in_file_.seekg( in_file_lastline_ );

      while ( !in_file_.eof() ) {
        std::getline( in_file_, line );
        std::stringstream str( trim( line ) );
        if ( str.str().empty() ) continue;

        // extract the list of properties
        std::string buffer;
        ValuesCollection values;
        while ( str >> buffer ) { values.push_back( buffer ); }
        try {
          auto elem = parseElement( values );
          if ( !elem ) continue;
          if ( fabs( elem->s() )>raw_beamline_->maxLength() ) {
            if ( has_next_element_ ) throw Exception( __PRETTY_FUNCTION__, "Finished to parse the beamline", Info );
            has_next_element_ = true;
          }
          raw_beamline_->addElement( elem );
        } catch ( Exception& e ) {
          if ( e.type() == Info ) break; // finished to parse
          e.dump();
        }
      }
    }

    std::shared_ptr<Element::ElementBase>
    MADX::parseElement( const ValuesCollection& values )
    {
      // first check if the "correct" number of element properties is parsed
      if ( values.size() != elements_fields_.size() ) {
        throw Exception( __PRETTY_FUNCTION__, Form( "MAD-X output seems corrupted!\n\t"
                                                    "Element %s has %d fields when %d are expected.",
                                                    trim( values.at( 0 ) ).c_str(), values.size(), elements_fields_.size() ), Fatal );
      }

      // then perform the 3-fold matching key <-> value <-> value type
      ParametersMap::Ordered<float> elem_map_floats;
      ParametersMap::Ordered<std::string> elem_map_str;
      for ( unsigned short i=0; i<values.size(); i++ ) {
        const std::string key = elements_fields_.key( i ),
                          value = values.at( i );
        const ValueType type = elements_fields_.value( i );
        switch ( type ) {
          case String: elem_map_str.add( key, value ); break;
          case Float: elem_map_floats.add( key, atof( value.c_str() ) ); break;
          case Unknown: default: {
            throw Exception( __PRETTY_FUNCTION__, Form( "MAD-X predicts an unknown-type optics element parameter:\n\t (%s) for %s", elements_fields_.key( i ).c_str(), trim( values.at( 0 ) ).c_str() ), JustWarning );
          } break;
        }
      }

      const std::string name = trim( elem_map_str.get( "name" ) );
      const float s_abs = elem_map_floats.get( "s" ),
                  length = elem_map_floats.get( "l" );


      const float s = s_abs-s_offset_-length;

      // convert the element type from string to object
      const Element::Type elemtype = ( elem_map_str.hasKey( "keyword" ) )
        ? findElementTypeByKeyword( lowercase( trim( elem_map_str.get( "keyword" ) ) ) )
        : findElementTypeByName( name );

      std::shared_ptr<Element::ElementBase> elem;

      try {
        // create the element
        switch ( elemtype ) {
          case Element::aGenericQuadrupole: {
            if ( length <= 0. ) throw Exception( __PRETTY_FUNCTION__, Form( "Trying to add a quadrupole with invalid length (l=%.2e m)", length ), JustWarning );

            const float k1l = elem_map_floats.get( "k1l" ),
                        mag_str_k = -k1l/length;
            if ( k1l > 0 ) { elem.reset( new Element::HorizontalQuadrupole( name, s, length, mag_str_k ) ); }
            else           { elem.reset( new Element::VerticalQuadrupole( name, s, length, mag_str_k ) ); }
          } break;
          case Element::aRectangularDipole: {
            const float k0l = elem_map_floats.get( "k0l" );
            if ( length <= 0. ) throw Exception( __PRETTY_FUNCTION__, Form( "Trying to add a rectangular dipole with invalid length (l=%.2e m)", length ), JustWarning );
            if ( k0l == 0. ) throw Exception( __PRETTY_FUNCTION__, Form( "Trying to add a rectangular dipole (%s) with k0l=%.2e", name.c_str(), k0l ), JustWarning );

            const float mag_strength = dir_*k0l/length;
            elem.reset( new Element::RectangularDipole( name, s, length, mag_strength ) );
          } break;
          case Element::aSectorDipole: {
            const float k0l = elem_map_floats.get( "k0l" );
            if ( length <= 0. ) throw Exception( __PRETTY_FUNCTION__, Form( "Trying to add a sector dipole with invalid length (l=%.2e m)", length ), JustWarning );
            if ( k0l == 0. ) throw Exception( __PRETTY_FUNCTION__, Form( "Trying to add a sector dipole (%s) with k0l=%.2e", name.c_str(), k0l ), JustWarning );

            const float mag_strength = dir_*k0l/length;
            elem.reset( new Element::SectorDipole( name, s, length, mag_strength ) );
          } break;
          case Element::anHorizontalKicker: {
            const float hkick = elem_map_floats.get( "hkick" );
            //if ( hkick == 0. ) throw Exception( __PRETTY_FUNCTION__, Form( "Trying to add a horizontal kicker (%s) with kick=%.2e", name.c_str(), hkick ), JustWarning );
            if ( hkick == 0. ) return 0;

            elem.reset( new Element::HorizontalKicker( name, s, length, hkick ) );
          } break;
          case Element::aVerticalKicker: {
            const float vkick = elem_map_floats.get( "vkick" );
            //if ( vkick == 0. ) throw Exception( __PRETTY_FUNCTION__, Form( "Trying to add a vertical kicker (%s) with kick=%.2e", name.c_str(), vkick ), JustWarning );
            if ( vkick == 0. ) return 0;

            elem.reset( new Element::VerticalKicker( name, s, length, vkick ) );
          } break;
          case Element::aRectangularCollimator: {
            elem.reset( new Element::RectangularCollimator( name, s, length ) );
          } break;
          case Element::aMarker: { elem.reset( new Element::Marker( name, s, length ) ); } break;
          case Element::aMonitor:
          case Element::anInstrument: {
            raw_beamline_->addMarker( Element::Marker( name, s, length ) );
            has_next_element_ = false;
            return 0;
          } break;
          case Element::aDrift: {
            previous_relpos_ = TwoVector( elem_map_floats.get( "x" ), elem_map_floats.get( "y" ) );
            previous_disp_ = TwoVector( elem_map_floats.get( "dx" ), elem_map_floats.get( "dy" ) );
            previous_beta_ = TwoVector( elem_map_floats.get( "betx" ), elem_map_floats.get( "bety" ) );
            has_next_element_ = false;
            return 0;
          } break;
          default: break;
        }

        // did not successfully create and populate a new element
        if ( !elem ) return elem;

        const TwoVector relpos( elem_map_floats.get( "x" ), elem_map_floats.get( "y" ) );
        //const TwoVector relpos;
        const int direction = 1; //FIXME
        if ( direction<0 ) {
          const TwoVector disp( elem_map_floats.get( "dx" ), elem_map_floats.get( "dy" ) ),
                          beta( elem_map_floats.get( "betx" ), elem_map_floats.get( "bety" ) );
          elem->setRelativePosition( relpos );
          elem->setDispersion( disp );
          elem->setBeta( beta );
        }
        else {
          elem->setRelativePosition( previous_relpos_ );
          elem->setDispersion( previous_disp_ );
          elem->setBeta( previous_beta_ );
        }

        // associate the aperture type to the element
        if ( elem_map_str.hasKey( "apertype" ) ) {
          const std::string aper_type = lowercase( trim( elem_map_str.get( "apertype" ) ) );
          const Aperture::Type apertype = findApertureTypeByApertype( aper_type );
          const float aper_1 = elem_map_floats.get( "aper_1" ),
                      aper_2 = elem_map_floats.get( "aper_2" ),
                      aper_3 = elem_map_floats.get( "aper_3" ),
                      aper_4 = elem_map_floats.get( "aper_4" ); // MAD-X provides it in m
          switch ( apertype ) {
            case Aperture::aRectEllipticAperture: { elem->setAperture( std::make_shared<Aperture::RectEllipticAperture>( aper_1, aper_2, aper_3, aper_4, relpos ) ); } break;
            case Aperture::aRectCircularAperture: { elem->setAperture( std::make_shared<Aperture::RectEllipticAperture>( aper_1, aper_2, aper_3, aper_3, relpos ) ); } break;
            case Aperture::aCircularAperture:     { elem->setAperture( std::make_shared<Aperture::CircularAperture>( aper_1, relpos ) ); } break;
            case Aperture::aRectangularAperture:  { elem->setAperture( std::make_shared<Aperture::RectangularAperture>( aper_1, aper_2, relpos ) ); } break;
            case Aperture::anEllipticAperture:    { elem->setAperture( std::make_shared<Aperture::EllipticAperture>( aper_1, aper_2, relpos ) ); } break;
            default: break;
          }
        }

      } catch ( Exception& e ) { e.dump(); }
      return elem;
    }

    Element::Type
    MADX::findElementTypeByName( std::string name )
    {
      try {
        if ( std::regex_match( name,       rgx_drift_name_ ) ) return Element::aDrift;
        if ( std::regex_match( name,     rgx_quadrup_name_ ) ) return Element::aGenericQuadrupole; //FIXME
        if ( std::regex_match( name, rgx_sect_dipole_name_ ) ) return Element::aSectorDipole;
        if ( std::regex_match( name, rgx_rect_dipole_name_ ) ) return Element::aRectangularDipole;
        if ( std::regex_match( name,          rgx_ip_name_ ) ) return Element::aMarker;
        if ( std::regex_match( name,     rgx_monitor_name_ ) ) return Element::aMonitor;
        if ( std::regex_match( name,    rgx_rp_horiz_name_ ) ) return Element::aRectangularCollimator;
        if ( std::regex_match( name,     rgx_rp_vert_name_ ) ) return Element::aRectangularCollimator;
        if ( std::regex_match( name,   rgx_rect_coll_name_ ) ) return Element::aRectangularCollimator;
      } catch ( std::regex_error& e ) {
        throw Exception( __PRETTY_FUNCTION__, Form( "Error while retrieving the element type!\n\t%s", e.what() ), Fatal );
      }
      return Element::anInvalidElement;
    }

    Element::Type
    MADX::findElementTypeByKeyword( std::string keyword )
    {
      if ( keyword.compare(      "marker" ) == 0 ) return Element::aMarker;
      if ( keyword.compare(       "drift" ) == 0 ) return Element::aDrift;
      if ( keyword.compare(     "monitor" ) == 0 ) return Element::aMonitor;
      if ( keyword.compare(  "quadrupole" ) == 0 ) return Element::aGenericQuadrupole;
      if ( keyword.compare(   "sextupole" ) == 0 ) return Element::aSextupole;
      if ( keyword.compare(   "multipole" ) == 0 ) return Element::aMultipole;
      if ( keyword.compare(       "sbend" ) == 0 ) return Element::aSectorDipole;
      if ( keyword.compare(       "rbend" ) == 0 ) return Element::aRectangularDipole;
      if ( keyword.compare(     "hkicker" ) == 0 ) return Element::anHorizontalKicker;
      if ( keyword.compare(     "vkicker" ) == 0 ) return Element::aVerticalKicker;
      if ( keyword.compare( "rcollimator" ) == 0 ) return Element::aRectangularCollimator;
      if ( keyword.compare( "ecollimator" ) == 0 ) return Element::anEllipticalCollimator;
      if ( keyword.compare( "ccollimator" ) == 0 ) return Element::aCircularCollimator;
      if ( keyword.compare( "placeholder" ) == 0 ) return Element::aPlaceholder;
      if ( keyword.compare(  "instrument" ) == 0 ) return Element::anInstrument;
      if ( keyword.compare(    "solenoid" ) == 0 ) return Element::aSolenoid;
      return Element::anInvalidElement;
    }

    Aperture::Type
    MADX::findApertureTypeByApertype( std::string apertype )
    {
      if ( apertype.compare(        "none" ) == 0 ) return Aperture::anInvalidAperture;
      if ( apertype.compare(   "rectangle" ) == 0 ) return Aperture::aRectangularAperture;
      if ( apertype.compare(     "ellipse" ) == 0 ) return Aperture::anEllipticAperture;
      if ( apertype.compare(      "circle" ) == 0 ) return Aperture::aCircularAperture;
      if ( apertype.compare( "rectellipse" ) == 0 ) return Aperture::aRectEllipticAperture;
      if ( apertype.compare(   "racetrack" ) == 0 ) return Aperture::aRaceTrackAperture;
      if ( apertype.compare(     "octagon" ) == 0 ) return Aperture::anOctagonalAperture;
      return Aperture::anInvalidAperture;
    }

    std::ostream&
    operator<<( std::ostream& os, const IO::MADX::ValueType& type )
    {
      switch ( type ) {
        case IO::MADX::Unknown: os << "unknown"; break;
        case IO::MADX::String:  os <<  "string"; break;
        case IO::MADX::Float:   os <<   "float"; break;
        case IO::MADX::Integer: os << "integer"; break;
      }
      return os;
    }
  } // namespace IO
} // namespace Hector
