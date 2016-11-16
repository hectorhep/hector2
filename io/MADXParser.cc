#include "MADXParser.h"

namespace Parser
{
  std::regex MADX::rgx_str_( "^\\%[0-9]?[0-9]?s$" );

  MADX::MADX( const char* filename, int direction, float max_s ) :
    dir_( direction/abs( direction ) )
  {
    in_file_ = std::ifstream( filename );

    try {
      parseHeader();

      header_float_.dump();

      beamline_.setLength( max_s );
      if ( max_s<0. and header_float_.hasKey( "length" ) ) beamline_.setLength( header_float_.get( "length" ) );

      parseElementsFields();
      parseElements();

      for ( Beamline::ElementsMap::const_iterator elem=beamline_.begin(); elem!=beamline_.end(); elem++ ) {
        std::cout << "---> " << *elem << std::endl;
      }

    } catch ( Exception& e ) { e.dump(); }

    //header_str_.dump();
  }

  MADX::~MADX()
  {
    if ( in_file_.is_open() ) in_file_.close();
  }

  void
  MADX::parseHeader()
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
  MADX::parseElementsFields()
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
  MADX::parseElements()
  {
    // parse the optics elements and their characteristics
    std::string line;

    in_file_.seekg( in_file_lastline_ );

    // quantities needed whenever direction == 1 (FIXME)
    float previous_x( 0. ), previous_y( 0. ),
          previous_dx( 0. ), previous_dy( 0. ),
          previous_betax( 0. ), previous_betay( 0. );

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

      // then perform the 3-fold matching key <-> value <-> value type
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

        const std::string name = trim( elem_map_str.get( "name" ) );
        const Element::ElementBase::Type elemtype = Element::Dictionary::get()->elementType(
          lowercase( trim( elem_map_str.get( "keyword" ) ) )
        );

        std::cout << name << " ===> " << elemtype << std::endl;

        const float s = elem_map_floats.get( "s" ),
                    length = elem_map_floats.get( "l" );

        // create the element
        switch ( elemtype ) {
          case Element::ElementBase::Quadrupole: {
            const float k1l = elem_map_floats.get( "k1l" ),
                        mag_str_k = -k1l/length;
            if ( k1l>0 ) {
              Element::HorizontalQuadrupole quad( name );
              quad.setS( s );
              quad.setLength( length );
              quad.setMagneticStrength( mag_str_k );
              beamline_.addElement( &quad );
             }
            else {
              Element::VerticalQuadrupole quad( name );
              quad.setS( s );
              quad.setLength( length );
              quad.setMagneticStrength( mag_str_k );
              beamline_.addElement( &quad );
            }
          } break;
          case Element::ElementBase::RectangularDipole: {
            Element::RectangularDipole dip( name );
            dip.setS( s );
            dip.setLength( length );
            dip.setMagneticStrength( dir_*elem_map_floats.get( "k0l" )/length );
            beamline_.addElement( &dip );
          } break;
          case Element::ElementBase::SectorDipole: {
            Element::SectorDipole dip( name );
            dip.setS( s );
            dip.setLength( length );
            dip.setMagneticStrength( dir_*elem_map_floats.get( "k0l" )/length );
            beamline_.addElement( &dip );
          } break;
          case Element::ElementBase::HorizontalKicker: {
            Element::HorizontalKicker kck( name );
            kck.setS( s );
            kck.setLength( length );
            kck.setMagneticStrength( elem_map_floats.get( "hkick" ) );
            beamline_.addElement( &kck );
          } break;
          case Element::ElementBase::VerticalKicker: {
            Element::VerticalKicker kck( name );
            kck.setS( s );
            kck.setLength( length );
            kck.setMagneticStrength( elem_map_floats.get( "vkick" ) );
            beamline_.addElement( &kck );
          } break;
          case Element::ElementBase::RectangularCollimator: {
            Element::RectangularCollimator col( name );
            col.setS( s );
            col.setLength( length );
            beamline_.addElement( &col );
          } break;
          case Element::ElementBase::Drift: {
            Element::Drift dr( elemtype, name );
            dr.setS( s );
            dr.setLength( length );
            beamline_.addElement( &dr );

            previous_x = elem_map_floats.get( "x" );
            previous_y = elem_map_floats.get( "y" );
            previous_dx = elem_map_floats.get( "dx" );
            previous_dy = elem_map_floats.get( "dy" );
            previous_betax = elem_map_floats.get( "betx" );
            previous_betax = elem_map_floats.get( "bety" );
          } break;
          default: break;
        }

        Element::ElementBase* elem = beamline_.getElement( name );
        if ( !elem ) continue; // beamline element was not found

        if ( dir_<0 ) {
          elem->setRelX( elem_map_floats.get( "x" ) );
          elem->setRelY( elem_map_floats.get( "y" ) );
          elem->setDX( elem_map_floats.get( "dx" ) );
          elem->setDY( elem_map_floats.get( "dy" ) );
          elem->setBetaX( elem_map_floats.get( "betx" ) );
          elem->setBetaY( elem_map_floats.get( "bety" ) );
        }
        else {
          elem->setRelX( previous_x );
          elem->setRelY( previous_y );
          elem->setDX( previous_dx );
          elem->setDY( previous_dy );
          elem->setBetaX( previous_betax );
          elem->setBetaY( previous_betay );
        }

        // associate the aperture type to the element
        const Element::ApertureBase::Type apertype = Element::Dictionary::get()->apertureType(
          lowercase( trim( elem_map_str.get( "apertype" ) ) )
        );
        Element::ApertureBase* aperture = 0;
        const float aper_1 = elem_map_floats.get( "aper_1" )*1.e6,
                    aper_2 = elem_map_floats.get( "aper_2" )*1.e6,
                    aper_3 = elem_map_floats.get( "aper_3" )*1.e6,
                    aper_4 = elem_map_floats.get( "aper_4" )*1.e6; // rad -> urad
        switch ( apertype ) {
          case Element::ApertureBase::RectElliptic: aperture = new Element::RectEllipticAperture( aper_1, aper_2, aper_3, aper_4 ); break;
          case Element::ApertureBase::Circular:     aperture = new Element::CircularAperture( aper_1 ); break;
          case Element::ApertureBase::Rectangular:  aperture = new Element::RectangularAperture( aper_1, aper_2 ); break;
          case Element::ApertureBase::Elliptic:     aperture = new Element::EllipticAperture( aper_1, aper_2 ); break;
          case Element::ApertureBase::None: break;
        }
        if ( aperture ) elem->setAperture( aperture );

      } catch ( Exception& e ) {
        e.dump();
        throw Exception( __PRETTY_FUNCTION__, "Failed to add a new element", Fatal );
      }

    }
  }

  std::ostream&
  operator<<( std::ostream& os, const Parser::MADX::ValueType& type )
  {
    switch ( type ) {
      case Parser::MADX::Unknown: os << "unknown"; break;
      case Parser::MADX::String: os << "string"; break;
      case Parser::MADX::Float: os << "float"; break;
      case Parser::MADX::Integer: os << "integer"; break;
    }
    return os;
  }
}

