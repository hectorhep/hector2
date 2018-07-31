#include "Hector/IO/TwissHandler.h"

#include "Hector/Core/Exception.h"

#include "Hector/Beamline/Beamline.h"

#include "Hector/Elements/Quadrupole.h"
#include "Hector/Elements/Dipole.h"
#include "Hector/Elements/Collimator.h"
#include "Hector/Elements/Kicker.h"
#include "Hector/Elements/Marker.h"

#include "Hector/Elements/EllipticAperture.h"
#include "Hector/Elements/CircularAperture.h"
#include "Hector/Elements/RectangularAperture.h"
#include "Hector/Elements/RectEllipticAperture.h"

#include <time.h>

namespace Hector
{
/*  namespace ParametersMap
  {
    //template class Unordered<IO::Twiss::ValueType>;
    template const std::string Unordered<IO::Twiss::ValueType>::key( const size_t i ) const;
  }*/
  namespace IO
  {
    std::regex Twiss::rgx_typ_( "^\\%[0-9]{0,}(s|le)$" );
    std::regex Twiss::rgx_hdr_( "^\\@ (\\w+) +\\%([0-9]+s|le) +\\\"?([^\"\\n]+)" );
    std::regex Twiss::rgx_elm_hdr_( "^\\s{0,}([\\*\\$])(.+)" );
    std::regex Twiss::rgx_drift_name_( "DRIFT\\_[0-9]+" );
    std::regex Twiss::rgx_quadrup_name_( "M[B,Q]\\w+\\d?\\.\\w?\\d[L,R]\\d(\\.B[1,2])?" );
    std::regex Twiss::rgx_sect_dipole_name_( "MB\\.[A-Z][0-9]{1,2}[L,R][0-9]\\.B[1,2]" );
    std::regex Twiss::rgx_rect_dipole_name_( "MB[A-Z0-9]{2,3}\\.*[B,R][0-9]" );
    std::regex Twiss::rgx_ip_name_( "IP[0-9]" );
    std::regex Twiss::rgx_monitor_name_( "BPM.+" );
    std::regex Twiss::rgx_rect_coll_name_( "T[C,A].*\\.\\d[L,R]\\d\\.?(B[1-9])?" );

    Twiss::Twiss( std::string filename, std::string ip_name, float max_s, float min_s ) :
      in_file_( filename ),
      ip_name_( ip_name ), min_s_( min_s )
    {
      if ( !in_file_.is_open() )
        throw Exception( __PRETTY_FUNCTION__, Form( "Failed to open the Twiss file \"%s\"\n\tPlease check the path!", filename.c_str() ), FatalError );
      parseHeader();

      raw_beamline_ = std::unique_ptr<Beamline>( new Beamline( max_s-min_s ) );
      if ( max_s < 0. && header_float_.hasKey( "length" ) ) raw_beamline_->setLength( header_float_.get( "length" ) );
      if ( header_float_.hasKey( "energy" ) && Parameters::get()->beamEnergy() != header_float_.get( "energy" ) ) {
        Parameters::get()->setBeamEnergy( header_float_.get( "energy" ) );
        PrintWarning( Form( "Beam energy changed to %.1f GeV to match Twiss optics parameters", Parameters::get()->beamEnergy() ) );
      }
      if ( header_float_.hasKey( "mass" ) && Parameters::get()->beamParticlesMass() != header_float_.get( "mass" ) ) {
        Parameters::get()->setBeamParticlesMass( header_float_.get( "mass" ) );
        PrintWarning( Form( "Beam particles mass changed to %.4f GeV to match Twiss optics parameters", Parameters::get()->beamParticlesMass() ) );
      }
      if ( header_float_.hasKey( "charge" ) && Parameters::get()->beamParticlesCharge() != static_cast<int>( header_float_.get( "charge" ) ) ) {
        Parameters::get()->setBeamParticlesCharge( static_cast<int>( header_float_.get( "charge" ) ) );
        PrintWarning( Form( "Beam particles charge changed to %d e to match Twiss optics parameters", Parameters::get()->beamParticlesCharge() ) );
      }

      parseElementsFields();

      // start by identifying the interaction point
      findInteractionPoint();

      // then parse all elements
      parseElements();

      beamline_ = Beamline::sequencedBeamline( raw_beamline_.get() );
    }

    Twiss::Twiss( const char* filename, const char* ip_name, float max_s, float min_s ) :
      Twiss( std::string( filename ), std::string( ip_name ), max_s, min_s ) {}

    Twiss::Twiss( const Twiss& rhs ) :
      interaction_point_( rhs.interaction_point_ ),
      ip_name_( rhs.ip_name_ ), min_s_( rhs.min_s_ )
    {}

    Twiss::Twiss( Twiss& rhs ) :
      beamline_( std::move( rhs.beamline_ ) ), raw_beamline_( std::move( rhs.raw_beamline_ ) ),
      interaction_point_( rhs.interaction_point_ ),
      ip_name_( rhs.ip_name_ ), min_s_( rhs.min_s_ )
    {}

    Beamline*
    Twiss::beamline() const
    {
      if ( !beamline_ ) {
        PrintWarning( "Sequenced beamline not computed from the Twiss file. "
                      "Retrieving the raw version. "
                      "You may encounter some numerical issues." );
        return raw_beamline_.get();
      }
      return beamline_.get();
    }

    void
    Twiss::printInfo() const
    {
      std::ostringstream os;
      os << "Twiss file successfully parsed. Metadata:";
      if ( header_str_.hasKey( "title" ) ) os << "\n\t Title: " << header_str_.get( "title" );
      if ( header_str_.hasKey( "origin" ) ) os << "\n\t Origin: " << trim( header_str_.get( "origin" ) );
      if ( header_float_.hasKey( "timestamp" ) ) {
        // C implementation for pre-gcc5 compilers
        time_t time = (long)header_float_.get( "timestamp" ); std::tm tm;
        char* time_chr = new char[100];
        strftime( time_chr, 100, "%c", localtime_r( &time, &tm ) );
        os << "\n\t Export date: " << time_chr;
        delete [] time_chr;
      }
      else if ( header_str_.hasKey( "date" ) || header_str_.hasKey( "time" ) )
        os << "\n\t Export date: " << trim( header_str_.get( "date" ) ) << " @ " << trim( header_str_.get( "time" ) );
      if ( header_float_.hasKey( "energy" ) ) os << "\n\t Simulated single beam energy: " << header_float_.get( "energy" ) << " GeV";
      if ( header_str_.hasKey( "sequence" ) ) os << "\n\t Sequence: " << header_str_.get( "sequence" );
      if ( header_str_.hasKey( "particle" ) ) os << "\n\t Beam particles: " << header_str_.get( "particle" );
      if ( header_float_.hasKey( "length" ) ) os << "\n\t Maximal beamline length: " << header_float_.get( "length" ) << " m";
      PrintInfo( os.str().c_str() );
    }

    std::map<std::string,std::string>
    Twiss::headerStrings() const
    {
      return header_str_.asMap();
    }

    std::map<std::string,float>
    Twiss::headerFloats() const
    {
      return header_float_.asMap();
    }

    void
    Twiss::parseHeader()
    {
      if ( !in_file_.is_open() )
        throw Exception( __PRETTY_FUNCTION__, "Twiss file is not opened nor ready for parsing!", FatalError );
      std::string line;
      while ( !in_file_.eof() ) {
        std::getline( in_file_, line );

        try {
          std::smatch match;
          if ( !std::regex_search( line, match, rgx_hdr_ ) ) break;

          const std::string key = lowercase( match.str( 1 ) );
          if ( match.str( 2 ) == "le" )
            header_float_.add( key, std::stod( match.str( 3 ) ) );
          else
            header_str_.add( key, match.str( 3 ) );

          // keep track of the last line read in the file
          in_file_lastline_ = in_file_.tellg();
        } catch ( std::regex_error& e ) {
          throw Exception( __PRETTY_FUNCTION__,
            Form( "Error at line %d while parsing the header!\n\t%s",
                  in_file_.tellg(), e.what() ), FatalError );
        }
      }
      // parse the Twiss file production timestamp
      if ( header_str_.hasKey( "date" ) ) {
        std::string date = trim( header_str_.get( "date" ) );
        std::string time = trim( ( header_str_.hasKey( "time" ) )
          ? header_str_.get( "time" )
          : "00.00.00" );
        struct std::tm tm = { 0 }; // fixes https://stackoverflow.com/questions/9037631
        //std::istringstream ss( date+" "+time ); ss >> std::get_time( &tm, "%d/%m/%y %H.%M.%S" ); // unfortunately only from gcc 5+...
        if ( strptime( ( date+" "+time+" CET" ).c_str(), "%d/%m/%y %H.%M.%S %z", &tm ) == nullptr ) {
          if ( mktime( &tm ) < 0 ) tm.tm_year += 100; // strong assumption that the Twiss file has been produced after 1970...
          header_float_.add( "timestamp", float( mktime( &tm ) ) );
        }
      }
    }

    void
    Twiss::parseElementsFields()
    {
      if ( !in_file_.is_open() )
        throw Exception( __PRETTY_FUNCTION__, "Twiss file is not opened nor ready for parsing!", FatalError );
      std::string line;

      in_file_.seekg( in_file_lastline_ );
      std::vector<std::string> list_names, list_types;

      while ( !in_file_.eof() ) {
        std::getline( in_file_, line );

        try {
          std::smatch match;
          if ( !std::regex_search( line, match, rgx_elm_hdr_ ) )
            break;

          std::string field;
          std::stringstream str( match.str( 2 ) );
          switch ( match.str( 1 )[0] ) {
            case '*': while ( str >> field ) list_names.emplace_back( lowercase( field ) ); break; // field names
            case '$': while ( str >> field ) list_types.emplace_back( field ); break; // field types
            default: break;
          }
          in_file_lastline_ = in_file_.tellg();
        } catch ( std::regex_error& e ) {
          throw Exception( __PRETTY_FUNCTION__,
            Form( "Error at line %d while parsing elements fields!\n\t%s",
                  in_file_.tellg(), e.what() ), FatalError );
        }
      }

      // perform the matching name <-> data type
      bool has_lists_matching = ( list_names.size() == list_types.size() );
      for ( unsigned short i = 0; i < list_names.size(); i++ ) {
        ValueType type = Unknown;
        try {
          std::smatch match;
          if ( has_lists_matching && std::regex_search( list_types.at( i ), match, rgx_typ_ ) ) {
            if ( match.str( 1 ) == "le" )
              type = Float;
            else if ( match.str( 1 ) == "s" )
              type = String;
          }
          elements_fields_.add( list_names.at( i ), type );
        } catch ( std::regex_error& e ) {
          throw Exception( __PRETTY_FUNCTION__,
            Form( "Error while performing the matching name-data!\n\t%s",
                  e.what() ), FatalError );
        }
      }
    }

    void
    Twiss::findInteractionPoint()
    {
      if ( !in_file_.is_open() )
        throw Exception( __PRETTY_FUNCTION__, "Twiss file is not opened nor ready for parsing!", FatalError );
      std::string line;
      in_file_.seekg( in_file_lastline_ );

      while ( !in_file_.eof() ) {
        std::getline( in_file_, line );
        std::stringstream str( trim( line ) );
        if ( str.str().length() == 0 ) continue;
        std::string buffer;
        ValuesCollection values;
        while ( str >> buffer ) values.push_back( buffer );
        // first check if the "correct" number of element properties is parsed
        if ( values.size() != elements_fields_.size() )
          throw Exception( __PRETTY_FUNCTION__,
            Form( "Twiss file seems corrupted!\n\t"
                  "Element %s at line %d has %d fields when %d are expected.",
                  trim( values.at( 0 ) ).c_str(), in_file_.tellg(),
                  values.size(), elements_fields_.size() ), FatalError );
        try {
          auto elem = parseElement( values );
          if ( !elem || elem->name() != ip_name_ ) continue;
          interaction_point_ = elem;
          raw_beamline_->setInteractionPoint( ThreeVector( elem->x(), elem->y(), 0. ) );
          break;
        } catch ( Exception& e ) {
          e.dump();
          throw Exception( __PRETTY_FUNCTION__,
            Form( "Failed to retrieve the interaction point with name=\"%s\"",
                  ip_name_.c_str() ), FatalError );
        }
      }
    }

    void
    Twiss::parseElements()
    {
      if ( !in_file_.is_open() )
        throw Exception( __PRETTY_FUNCTION__, "Twiss file is not opened nor ready for parsing!", FatalError );
      // parse the optics elements and their characteristics
      std::string line;

      if ( !interaction_point_ )
        throw Exception( __PRETTY_FUNCTION__,
          Form( "Interaction point \"%s\" has not been found in the beamline!",
                ip_name_.c_str() ), FatalError );

      in_file_.seekg( in_file_lastline_ ); // return to the first element line

      bool has_next_element = false;
      while ( !in_file_.eof() ) { // retrieve the next line from the Twiss file
        std::getline( in_file_, line );
        std::stringstream str( trim( line ) );
        if ( str.str().empty() )
          continue;

        // extract the list of properties
        std::string buffer;
        ValuesCollection values;
        while ( str >> buffer )
          values.emplace_back( buffer );
        try {
          auto elem = parseElement( values );
          if ( !elem || elem->type() == Element::aDrift )
            continue;
          elem->offsetS( -interaction_point_->s() );
          if ( elem->s() < min_s_ )
            continue;
          if ( elem->s()+elem->length() > raw_beamline_->maxLength() ) {
            if ( has_next_element )
              throw Exception( __PRETTY_FUNCTION__, "Finished to parse the beamline", Info, 20001 );
            if ( elem->type() != Element::anInstrument && elem->type() != Element::aDrift )
              has_next_element = true;
          }
          raw_beamline_->add( elem );
        } catch ( Exception& e ) {
          if ( e.errorNumber() != 20001 )
            e.dump();
          break; // finished to parse
        }
      }
      interaction_point_->setS( 0. ); // by convention
      raw_beamline_->add( interaction_point_ );
    }

    std::shared_ptr<Element::ElementBase>
    Twiss::parseElement( const ValuesCollection& values )
    {
      // first check if the "correct" number of element properties is parsed
      if ( values.size() != elements_fields_.size() )
        throw Exception( __PRETTY_FUNCTION__,
          Form( "Twiss file seems corrupted!\n\t"
                "Element %s has %d fields when %d are expected.",
                trim( values.at( 0 ) ).c_str(), values.size(), elements_fields_.size() ), FatalError );

      // then perform the 3-fold matching key <-> value <-> value type
      ParametersMap::Ordered<float> elem_map_floats;
      ParametersMap::Ordered<std::string> elem_map_str;
      for ( unsigned short i = 0; i < values.size(); i++ ) {
        const std::string key = elements_fields_.key( i ), value = values.at( i );
        const ValueType type = elements_fields_.value( i );
        switch ( type ) {
          case String:
            elem_map_str.add( key, value );
            break;
          case Float:
            elem_map_floats.add( key, std::stod( value ) );
            break;
          case Unknown: default: {
            throw Exception( __PRETTY_FUNCTION__,
              Form( "Twiss file predicts an unknown-type optics element parameter:\n\t (%s) for %s",
                    elements_fields_.key( i ).c_str(), trim( values.at( 0 ) ).c_str() ), JustWarning );
          } break;
        }
      }

      const std::string name = trim( elem_map_str.get( "name" ) );
      const float s = elem_map_floats.get( "s" ), length = elem_map_floats.get( "l" );

      // convert the element type from string to object
      const Element::Type elemtype = ( elem_map_str.hasKey( "keyword" ) )
        ? findElementTypeByKeyword( lowercase( trim( elem_map_str.get( "keyword" ) ) ) )
        : findElementTypeByName( name );

      std::shared_ptr<Element::ElementBase> elem;

      try {
        // create the element
        switch ( elemtype ) {
          case Element::aGenericQuadrupole: {
            if ( length <= 0. ) throw Exception( __PRETTY_FUNCTION__,
              Form( "Trying to add a quadrupole with invalid length (l=%.2e m)", length ), JustWarning );

            const double k1l = elem_map_floats.get( "k1l" );
            const double mag_str_k = -k1l/length;
            if ( k1l > 0 )
              elem.reset( new Element::HorizontalQuadrupole( name, s, length, mag_str_k ) );
            else
              elem.reset( new Element::VerticalQuadrupole( name, s, length, mag_str_k ) );
          } break;
          case Element::aRectangularDipole:
          case Element::aSectorDipole: {
            const double k0l = elem_map_floats.get( "k0l" );
            if ( length <= 0. )
              throw Exception( __PRETTY_FUNCTION__,
                Form( "Trying to add a dipole with invalid length (l=%.2e m)", length ),
                JustWarning );
            if ( k0l == 0. )
              throw Exception( __PRETTY_FUNCTION__,
                Form( "Trying to add a dipole (%s) with k0l=%.2e", name.c_str(), k0l ),
                JustWarning );

            const double mag_strength = k0l/length;
//            std::cout << name << "|" << s << "|" << mag_strength << std::endl;
            if ( elemtype == Element::aRectangularDipole )
              elem.reset( new Element::RectangularDipole( name, s, length, mag_strength ) );
            if ( elemtype == Element::aSectorDipole )
              elem.reset( new Element::SectorDipole( name, s, length, mag_strength ) );
          } break;
          case Element::anHorizontalKicker: {
            const double hkick = elem_map_floats.get( "hkick" );
            if ( hkick == 0. )
              return 0;
            elem.reset( new Element::HorizontalKicker( name, s, length, hkick ) );
          } break;
          case Element::aVerticalKicker: {
            const double vkick = elem_map_floats.get( "vkick" );
            if ( vkick == 0. )
              return 0;
            elem.reset( new Element::VerticalKicker( name, s, length, vkick ) );
          } break;
          case Element::aRectangularCollimator:
          case Element::anEllipticalCollimator:
          case Element::aCircularCollimator:
          case Element::aCollimator:
            elem.reset( new Element::Collimator( name, s, length ) );
            break;
          case Element::aMarker:
            elem.reset( new Element::Marker( name, s, length ) );
            break;
          case Element::anInstrument:
          case Element::aMonitor:
          case Element::aDrift:
            elem.reset( new Element::Drift( name, elemtype, s, length ) );
            break;
          default: break;
        }

        // did not successfully create and populate a new element
        if ( !elem )
          return elem;

        const TwoVector env_pos( elem_map_floats.get( "x" ), elem_map_floats.get( "y" ) );

        elem->setRelativePosition( env_pos );
        elem->setDispersion( TwoVector( elem_map_floats.get( "dx" ), elem_map_floats.get( "dy" ) ) );
        elem->setBeta( TwoVector( elem_map_floats.get( "betx" ), elem_map_floats.get( "bety" ) ) );

        // associate the aperture type to the element
        if ( elem_map_str.hasKey( "apertype" ) ) {
          const std::string aper_type = lowercase( trim( elem_map_str.get( "apertype" ) ) );
          const Aperture::Type apertype = findApertureTypeByApertype( aper_type );
          const double aper_1 = elem_map_floats.get( "aper_1" );
          const double aper_2 = elem_map_floats.get( "aper_2" );
          // MAD-X provides it in m
          switch ( apertype ) {
            case Aperture::aCircularAperture:
              elem->setAperture( std::make_shared<Aperture::CircularAperture>( aper_1, env_pos ) );
              break;
            case Aperture::aRectangularAperture:
              elem->setAperture( std::make_shared<Aperture::RectangularAperture>( aper_1, aper_2, env_pos ) );
              break;
            case Aperture::anEllipticAperture:
              elem->setAperture( std::make_shared<Aperture::EllipticAperture>( aper_1, aper_2, env_pos ) );
              break;
            case Aperture::aRectEllipticAperture: {
              const double aper_3 = elem_map_floats.get( "aper_3" );
              const double aper_4 = elem_map_floats.get( "aper_4" );
              elem->setAperture( std::make_shared<Aperture::RectEllipticAperture>( aper_1, aper_2, aper_3, aper_4, env_pos ) );
            } break;
            case Aperture::aRectCircularAperture: {
              const double aper_3 = elem_map_floats.get( "aper_3" );
              elem->setAperture( std::make_shared<Aperture::RectEllipticAperture>( aper_1, aper_2, aper_3, aper_3, env_pos ) );
            } break;
            default:
              break;
          }
        }

      } catch ( Exception& e ) { e.dump(); }
      return elem;
    }

    Element::Type
    Twiss::findElementTypeByName( std::string name )
    {
      try {
        if ( std::regex_match( name,       rgx_drift_name_ ) ) return Element::aDrift;
        if ( std::regex_match( name,     rgx_quadrup_name_ ) ) return Element::aGenericQuadrupole; //FIXME
        if ( std::regex_match( name, rgx_sect_dipole_name_ ) ) return Element::aSectorDipole;
        if ( std::regex_match( name, rgx_rect_dipole_name_ ) ) return Element::aRectangularDipole;
        if ( std::regex_match( name,          rgx_ip_name_ ) ) return Element::aMarker;
        if ( std::regex_match( name,     rgx_monitor_name_ ) ) return Element::aMonitor;
        if ( std::regex_match( name,   rgx_rect_coll_name_ ) ) return Element::aRectangularCollimator;
      } catch ( std::regex_error& e ) {
        throw Exception( __PRETTY_FUNCTION__, Form( "Error while retrieving the element type!\n\t%s", e.what() ), FatalError );
      }
      return Element::anInvalidElement;
    }

    Element::Type
    Twiss::findElementTypeByKeyword( std::string keyword )
    {
      if ( keyword ==      "marker" ) return Element::aMarker;
      if ( keyword ==       "drift" ) return Element::aDrift;
      if ( keyword ==     "monitor" ) return Element::aMonitor;
      if ( keyword ==  "quadrupole" ) return Element::aGenericQuadrupole;
      if ( keyword ==   "sextupole" ) return Element::aSextupole;
      if ( keyword ==   "multipole" ) return Element::aMultipole;
      if ( keyword ==       "sbend" ) return Element::aSectorDipole;
      if ( keyword ==       "rbend" ) return Element::aRectangularDipole;
      if ( keyword ==     "hkicker" ) return Element::anHorizontalKicker;
      if ( keyword ==     "vkicker" ) return Element::aVerticalKicker;
      if ( keyword ==  "collimator" ) return Element::aCollimator;
      if ( keyword == "rcollimator" ) return Element::aRectangularCollimator;
      if ( keyword == "ecollimator" ) return Element::anEllipticalCollimator;
      if ( keyword == "ccollimator" ) return Element::aCircularCollimator;
      if ( keyword == "placeholder" ) return Element::aPlaceholder;
      if ( keyword ==  "instrument" ) return Element::anInstrument;
      if ( keyword ==    "solenoid" ) return Element::aSolenoid;
      return Element::anInvalidElement;
    }

    Aperture::Type
    Twiss::findApertureTypeByApertype( std::string apertype )
    {
      if ( apertype ==        "none" ) return Aperture::anInvalidAperture;
      if ( apertype ==   "rectangle" ) return Aperture::aRectangularAperture;
      if ( apertype ==     "ellipse" ) return Aperture::anEllipticAperture;
      if ( apertype ==      "circle" ) return Aperture::aCircularAperture;
      if ( apertype == "rectellipse" ) return Aperture::aRectEllipticAperture;
      if ( apertype ==   "racetrack" ) return Aperture::aRaceTrackAperture;
      if ( apertype ==     "octagon" ) return Aperture::anOctagonalAperture;
      return Aperture::anInvalidAperture;
    }

    std::ostream&
    operator<<( std::ostream& os, const IO::Twiss::ValueType& type )
    {
      switch ( type ) {
        case IO::Twiss::Unknown: os << "unknown"; break;
        case IO::Twiss::String:  os <<  "string"; break;
        case IO::Twiss::Float:   os <<   "float"; break;
        case IO::Twiss::Integer: os << "integer"; break;
      }
      return os;
    }
  } // namespace IO
} // namespace Hector
