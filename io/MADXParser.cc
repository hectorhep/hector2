#include "MADXParser.h"

namespace Hector
{
  namespace Parser
  {
    std::regex MADX::rgx_typ_( "^\\%[0-9]{0,}(s|le)$" );
    std::regex MADX::rgx_hdr_( "^\\@ (\\w+) +\\%([0-9]+s|le) +\\\"?([^\"\\n]+)" );
    std::regex MADX::rgx_elm_hdr_( "^\\s{0,}([\\*\\$])(.+)" );
    std::regex MADX::rgx_rp_vert_name_( "XRPV\\.[0-9a-zA-Z]{4}\\.B[1,2]" );
    std::regex MADX::rgx_rp_horiz_name_( "XRPH\\.[0-9a-zA-Z]{4}\\.B[1,2]" );

    MADX::MADX( const char* filename, const char* ip_name, int direction, float max_s ) :
      beamline_( 0 ), raw_beamline_( 0 ), dir_( direction/abs( direction ) ),
      ip_name_( ip_name ), s_offset_( 0. ), found_interaction_point_( false ),
      has_next_element_( false )
    {
      in_file_ = std::ifstream( filename );

      try {
        if ( !in_file_.is_open() ) throw Exception( __PRETTY_FUNCTION__, Form( "Failed to open Twiss file \"%s\"\n\tCheck the path!", filename ), Fatal );
        parseHeader();

        raw_beamline_ = new Beamline( max_s );
        if ( max_s<0. and header_float_.hasKey( "length" ) ) raw_beamline_->setLength( header_float_.get( "length" ) );
        if ( header_float_.hasKey( "energy" ) and Parameters::beam_energy!=header_float_.get( "energy" ) ) {
          Parameters::beam_energy = header_float_.get( "energy" );
          PrintWarning( Form( "Beam energy changed to %.1f GeV to match the MAD-X optics parameters", Parameters::beam_energy ) );
        }
        if ( header_float_.hasKey( "mass" ) and Parameters::beam_particles_mass!=header_float_.get( "mass" ) ) {
          Parameters::beam_particles_mass = header_float_.get( "mass" );
          PrintWarning( Form( "Beam particles mass changed to %.4f GeV to match the MAD-X optics parameters", Parameters::beam_particles_mass ) );
        }
        if ( header_float_.hasKey( "charge" ) and Parameters::beam_particles_charge!=static_cast<int>( header_float_.get( "charge" ) ) ) {
          Parameters::beam_particles_charge = static_cast<int>( header_float_.get( "charge" ) );
          PrintWarning( Form( "Beam particles charge changed to %d e to match the MAD-X optics parameters", Parameters::beam_particles_charge ) );
        }

        parseElementsFields();

        // start by identifying the interaction point
        findInteractionPoint();

        // then parse all elements
        parseElements();

        beamline_ = Beamline::sequencedBeamline( raw_beamline_ );

      } catch ( Exception& e ) { e.dump(); }
    }

    MADX::~MADX()
    {
      if ( in_file_.is_open() ) in_file_.close();
      if ( beamline_ ) delete beamline_;
      if ( raw_beamline_ ) delete raw_beamline_;
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
      for ( Elements::const_iterator it=raw_beamline_->begin(); it!=raw_beamline_->end(); it++ ) {
        Element::ElementBase* elem = *( it );
        if ( std::regex_match( elem->name(), rgx_rp_horiz_name_ ) ) {
          if ( type==allPots or type==horizontalPots ) out.push_back( elem );
        }
        if ( std::regex_match( elem->name(), rgx_rp_vert_name_ ) ) {
          if ( type==allPots or type==verticalPots ) out.push_back( elem );
        }
      }
      return out;
    }

    void
    MADX::parseHeader()
    {
      std::string line;
      while ( !in_file_.eof() ) {
        std::getline( in_file_, line );

        std::smatch match;
        if ( !std::regex_search( line, match, rgx_hdr_ ) ) return;

        const std::string key = lowercase( match.str( 1 ) );
        if ( match.str( 2 )=="le" ) header_float_.add( key, atof( match.str( 3 ).c_str() ) );
        else header_str_.add( key, match.str( 3 ) );

        // keep track of the last line read in the file
        in_file_lastline_ = in_file_.tellg();
      }
    }

    void
    MADX::parseElementsFields()
    {
      std::string line;

      in_file_.seekg( in_file_lastline_ );
      std::vector<std::string> list_names, list_types;

      while ( !in_file_.eof() ) {
        std::getline( in_file_, line );

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
      }

      // perform the matching name <-> data type
      bool has_lists_matching = ( list_names.size()==list_types.size() );
      for ( unsigned short i=0; i<list_names.size(); i++ ) {
        ValueType type = Unknown;
        std::smatch match;
        if ( has_lists_matching and std::regex_search( list_types.at( i ), match, rgx_typ_ ) ) {
          if ( match.str( 1 )=="le" ) type = Float;
          else if ( match.str( 1 )=="s" ) type = String;
        }
        elements_fields_.add( list_names.at( i ), type );
      }
    }

    void
    MADX::findInteractionPoint()
    {
      std::string line;
      in_file_.seekg( in_file_lastline_ );

      while ( !in_file_.eof() ) {
        std::getline( in_file_, line );
        std::stringstream str( trim( line ) );
        if ( str.str().length()==0 ) continue;
        std::string buffer;
        ValuesCollection values;
        while ( str.good() ) { str >> buffer; values.push_back( buffer ); }
        // first check if the "correct" number of element properties is parsed
        if ( values.size()!=elements_fields_.size() ) {
          throw Exception( __PRETTY_FUNCTION__, Form( "MAD-X output seems corrupted!\n\tElement %s has %d fields when %d are expected.", trim( values.at( 0 ) ).c_str(), values.size(), elements_fields_.size() ), Fatal );
        }
        try {
          Element::ElementBase* elem = parseElement( values );
          if ( elem==0 ) continue;
          if ( elem->name()==ip_name_ ) {
            found_interaction_point_ = true;
            s_offset_ = elem->s();
            raw_beamline_->addElement( elem, true );
            return;
          }
          delete elem;
        } catch ( Exception& e ) {
          e.dump();
          throw Exception( __PRETTY_FUNCTION__, Form( "Failed to retrieve the interaction point with name=\"%s\"", ip_name_.c_str() ), Fatal );
        }
      }
    }

    void
    MADX::parseElements()
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
        ValuesCollection values;
        while ( str.good() ) { str >> buffer; values.push_back( buffer ); }
        Element::ElementBase* elem = 0;
        try {
          elem = parseElement( values );
          if ( !elem ) continue;
          if ( fabs( elem->s() )>raw_beamline_->maxLength() ) {
            if ( has_next_element_ ) throw Exception( __PRETTY_FUNCTION__, "Finished to parse the beamline", Info );
            has_next_element_ = true;
          }
          raw_beamline_->addElement( elem, true );
        } catch ( Exception& e ) {
          if ( e.type()==Info ) break; // finished to parse
          e.dump();
        }
      }
    }

    Element::ElementBase*
    MADX::parseElement( const ValuesCollection& values )
    {
      Element::ElementBase* elem = 0;

      // first check if the "correct" number of element properties is parsed
      if ( values.size()!=elements_fields_.size() ) {
        throw Exception( __PRETTY_FUNCTION__, Form( "MAD-X output seems corrupted!\n\tElement %s has %d fields when %d are expected.", trim( values.at( 0 ) ).c_str(), values.size(), elements_fields_.size() ), Fatal );
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
      const std::string elem_type = lowercase( trim( elem_map_str.get( "keyword" ) ) );
      const Element::Type elemtype = ElementDictionary::get().elementType( elem_type );

      try {
        // create the element
        switch ( elemtype ) {
          case Element::aGenericQuadrupole: {

            const float k1l = elem_map_floats.get( "k1l" ),
                        mag_str_k = -k1l/length;
            if ( k1l>0 ) { elem = new Element::HorizontalQuadrupole( name, s, length, mag_str_k ); }
            else         { elem = new Element::VerticalQuadrupole( name, s, length, mag_str_k ); }
          } break;
          case Element::aRectangularDipole: {
            const float k0l = elem_map_floats.get( "k0l" );
            if ( k0l==0. ) throw Exception( __PRETTY_FUNCTION__, Form( "Trying to add a rectangular dipole (%s) with k0l=%.2e", name.c_str(), k0l ), JustWarning );

            const float mag_strength = dir_*k0l/length;
            elem = new Element::RectangularDipole( name, s, length, mag_strength );
          } break;
          case Element::aSectorDipole: {
            const float k0l = elem_map_floats.get( "k0l" );
            if ( k0l==0. ) throw Exception( __PRETTY_FUNCTION__, Form( "Trying to add a sector dipole (%s) with k0l=%.2e", name.c_str(), k0l ), JustWarning );

            const float mag_strength = dir_*k0l/length;
            elem = new Element::SectorDipole( name, s, length, mag_strength );
          } break;
          case Element::anHorizontalKicker: {
            const float hkick = elem_map_floats.get( "hkick" );
            //if ( hkick==0. ) throw Exception( __PRETTY_FUNCTION__, Form( "Trying to add a horizontal kicker (%s) with kick=%.2e", name.c_str(), hkick ), JustWarning );
            if ( hkick==0. ) return 0;

            elem = new Element::HorizontalKicker( name, s, length, hkick );
          } break;
          case Element::aVerticalKicker: {
            const float vkick = elem_map_floats.get( "vkick" );
            //if ( vkick==0. ) throw Exception( __PRETTY_FUNCTION__, Form( "Trying to add a vertical kicker (%s) with kick=%.2e", name.c_str(), vkick ), JustWarning );
            if ( vkick==0. ) return 0;

            elem = new Element::VerticalKicker( name, s, length, vkick );
          } break;
          case Element::aRectangularCollimator: {
            elem = new Element::RectangularCollimator( name, s, length );
          } break;
          case Element::aMarker: { elem = new Element::Marker( name, s, length ); } break;
          case Element::aMonitor:
          case Element::anInstrument: {
            raw_beamline_->addMarker( Element::Marker( name, s, length ) );
            has_next_element_ = false;
            return 0;
          } break;
          case Element::aDrift: {
            previous_relpos_ = CLHEP::Hep2Vector( elem_map_floats.get( "x" ), elem_map_floats.get( "y" ) );
            previous_disp_ = CLHEP::Hep2Vector( elem_map_floats.get( "dx" ), elem_map_floats.get( "dy" ) );
            previous_beta_ = CLHEP::Hep2Vector( elem_map_floats.get( "betx" ), elem_map_floats.get( "bety" ) );
            has_next_element_ = false;
            return 0;
          } break;
          default: break;
        }

        // did not successfully create and populate a new element
        if ( !elem ) return 0;

        const CLHEP::Hep2Vector relpos( elem_map_floats.get( "x" ), elem_map_floats.get( "y" ) );
        //const CLHEP::Hep2Vector relpos;
        const int direction = 1; //FIXME
        if ( direction<0 ) {
          const CLHEP::Hep2Vector disp( elem_map_floats.get( "dx" ), elem_map_floats.get( "dy" ) ),
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
        const std::string aper_type = lowercase( trim( elem_map_str.get( "apertype" ) ) );
        const Aperture::Type apertype = ElementDictionary::get().apertureType( aper_type );
        const float aper_1 = elem_map_floats.get( "aper_1" ),
                    aper_2 = elem_map_floats.get( "aper_2" ),
                    aper_3 = elem_map_floats.get( "aper_3" ),
                    aper_4 = elem_map_floats.get( "aper_4" ); // MAD-X provides it in m
        switch ( apertype ) {
          case Aperture::aRectEllipticAperture: { elem->setAperture( new Aperture::RectEllipticAperture( aper_1, aper_2, aper_3, aper_4, relpos ), true ); } break;
          case Aperture::aCircularAperture:     { elem->setAperture( new Aperture::CircularAperture( aper_1, relpos ), true ); } break;
          case Aperture::aRectangularAperture:  { elem->setAperture( new Aperture::RectangularAperture( aper_1, aper_2, relpos ), true ); } break;
          case Aperture::anEllipticAperture:    { elem->setAperture( new Aperture::EllipticAperture( aper_1, aper_2, relpos ), true ); } break;
          case Aperture::anInvalidType: break;
        }

      } catch ( Exception& e ) { e.dump(); }
      return elem;
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
}

