#include "MadXparser.h"

namespace Parser
{
  std::regex MadX::rgx_str_( "^\\%[0-9]?[0-9]?s$" );

  MadX::MadX( const char* filename, int direction ) :
    dir_( direction/abs( direction ) )
  {
    in_file_ = std::ifstream( filename );

    try {
      parseHeader();

      header_float_.dump();

      if ( header_float_.hasKey( "length" ) ) beamline_.setLength( header_float_.get( "length" ) );

      parseElementsFields();
      parseElements();
    } catch ( Exception& e ) { e.dump(); }

    //header_str_.dump();
  }

  MadX::~MadX()
  {
    if ( in_file_.is_open() ) in_file_.close();
  }

  void
  MadX::parseHeader()
  {
    std::string line;
    while ( !in_file_.eof() ) {
      std::getline( in_file_, line );
      std::stringstream str( trim( line ) );
      if ( str.str().length()==0 ) continue;

      // only keep the lines starting by a '@' ; rest is for beamline elements
      const char first_chr = str.str().at( str.str().find_first_not_of( ' ' ) );
      if ( first_chr!='@' ) return;

      // keep track of the last line read in the file
      in_file_lastline_ = in_file_.tellg();

      // fill the header maps
      std::string chr, key, type, value;
      while ( str >> chr >> key >> type >> value ) {
        const bool is_string = std::regex_match( type, rgx_str_ ),
                   is_float = ( !is_string and type=="%le" ); //FIXME
        key = lowercase( key );
        if ( is_string ) header_str_.add( key, value );
        if ( is_float ) header_float_.add( key, atof( value.c_str() ) );
      }
    }
  }

  void
  MadX::parseElementsFields()
  {
    std::string line;

    in_file_.seekg( in_file_lastline_ );

    while ( !in_file_.eof() ) {
      std::getline( in_file_, line );
      std::stringstream str( trim( line ) );
      if ( str.str().length()==0 ) continue;
      const char first_chr = str.str().at( str.str().find_first_not_of( ' ' ) );

      // first look at the expected number of fields for each beamline element
      if ( first_chr!='*' ) break;

      std::vector<std::string> list_names, list_types;

      // fields names
      std::string buffer;
      while ( str.good() ) {
        str >> buffer;
        if ( trim( buffer )=="*" ) continue;
        list_names.push_back( lowercase( trim( buffer ) ) );
      }
      list_names.pop_back(); // remove the last element (overcounting)

      in_file_lastline_ = in_file_.tellg();

      std::string line2;
      std::getline( in_file_, line2 );
      std::stringstream str2( trim( line2 ) );
      if ( str2.str().length()!=0 ) {
        const char first_chr = str2.str().at( str2.str().find_first_not_of( ' ' ) );
        if ( first_chr=='$' ) {
          // list of fields stored below
          std::string buffer2;
          while ( str2.good() ) {
            str2 >> buffer2;
            if ( trim( buffer2 )=="$" ) continue;
            list_types.push_back( trim( buffer2 ) );
          }
          list_types.pop_back(); // remove the last element (overcounting)
        }

        in_file_lastline_ = in_file_.tellg();

      }

      // perform the matching name <-> data type
      bool has_lists_matching = ( list_names.size()==list_types.size() );
      for ( unsigned short i=0; i<list_names.size(); i++ ) {
        ValueType type = Unknown;
        if ( has_lists_matching ) {
          const std::string type_string = list_types.at( i );
          if ( std::regex_match( type_string, rgx_str_ ) ) type = String;
          if ( type_string=="%le" ) type = Float; //FIXME
        }
        elements_fields_.add( list_names.at( i ), type );
      }

      //elements_fields_.dump();
    }
  }

  void
  MadX::parseElements()
  {
    // parse the optics elements and their characteristics
    std::string line;

    in_file_.seekg( in_file_lastline_ );

    while ( !in_file_.eof() ) {
      std::getline( in_file_, line );
      std::stringstream str( trim( line ) );
      if ( str.str().length()==0 ) continue;

      // extract the list of properties
      std::string buffer;
      std::vector<std::string> values;
      while ( str.good() ) {
        str >> buffer;
        values.push_back( buffer );
      }

      // first check if the "correct" number of element properties is parsed
      if ( values.size()!=elements_fields_.size() ) {
        throw Exception( __PRETTY_FUNCTION__, Form( "MAD-X output seems corrupted!\n\tElement %s has %d fields when %d are expected.", trim( values.at( 0 ) ).c_str(), values.size(), elements_fields_.size() ), Fatal );
      }

      // then perform the 3-fold matching between key, value, and value type
      ParametersMap<float> elem_map_floats;
      ParametersMap<std::string> elem_map_str;
      for ( unsigned short i=0; i<values.size(); i++ ) {
        const std::string value = values.at( i ),
                          key = elements_fields_.key( i );
        const ValueType type = elements_fields_.value( i );
        switch ( type ) {
          case String: elem_map_str.add( key, value ); break;
          case Float: elem_map_floats.add( key, atof( value.c_str() ) ); break;
          case Unknown: default: {
            throw Exception( __PRETTY_FUNCTION__, Form( "MAD-X predicts an unknown-type optics element parameter:\n\t (%s) for %s", elements_fields_.at( i ).first.c_str(), trim( values.at( 0 ) ).c_str() ), JustWarning );
          } break;
        }
      }

      /*elem_map_str.dump();
      elem_map_floats.dump();*/

      try {

        const std::string name = elem_map_str.get( "name" );
        const Element::ElementBase::Type elemtype = Element::Dictionary::get()->elementType(
          lowercase( trim( elem_map_str.get( "keyword" ) ) )
        );
        const Element::ApertureBase::Type apertype = Element::Dictionary::get()->apertureType(
          lowercase( trim( elem_map_str.get( "apertype" ) ) )
        );
        const float s = elem_map_floats.get( "s" ),
                    length = elem_map_floats.get( "l" );

        switch ( elemtype ) {
          case Element::ElementBase::Quadrupole: {
            const float k1l = elem_map_floats.get( "k1l" ),
                        mag_str_k = -k1l/length;
            if ( k1l>0 ) { 
              Element::HorizontalQuadrupole quadr( name.c_str() );
              quadr.setS( s );
              quadr.setLength( length );
              quadr.setMagneticStrength( -k1l/length );
              beamline_.addElement( &quadr );
            }
            else {
              Element::VerticalQuadrupole quadr( name.c_str() );
              quadr.setS( s );
              quadr.setLength( length );
              quadr.setMagneticStrength( -k1l/length );
              beamline_.addElement( &quadr );
            }
          } break;
          case Element::ElementBase::RectangularDipole: {
            Element::RectangularDipole dip( name.c_str() );
            dip.setS( s );
            dip.setLength( length );
            dip.setMagneticStrength( dir_*elem_map_floats.get( "k0l" )/length );
            beamline_.addElement( &dip );
          } break;
          case Element::ElementBase::SectorDipole: {
            Element::SectorDipole dip( name.c_str() );
            dip.setS( s );
            dip.setLength( length );
            dip.setMagneticStrength( dir_*elem_map_floats.get( "k0l" )/length );
            beamline_.addElement( &dip );
          } break;
          case Element::ElementBase::HorizontalKicker: {
          } break;
          case Element::ElementBase::VerticalKicker: {
          } break;
          default: break;
        }
        //std::cout << " " << trim( name ) << " is a " << elemtype << std::endl;

      } catch ( Exception& e ) {
        e.dump();
        throw Exception( __PRETTY_FUNCTION__, "Failed to add a new element", Fatal );
      }

    }
  }

  std::ostream&
  operator<<( std::ostream& os, const Parser::MadX::ValueType& type )
  {
    switch ( type ) {
      case Parser::MadX::Unknown: os << "unknown"; break;
      case Parser::MadX::String: os << "string"; break;
      case Parser::MadX::Float: os << "float"; break;
      case Parser::MadX::Integer: os << "integer"; break;
    }
    return os;
  }
}

