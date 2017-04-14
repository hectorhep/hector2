#include "MADXParser.h"

namespace Hector
{
  namespace Parser
  {
    std::regex MADX::rgx_typ_( "^\\%[0-9]{0,}(s|le)$" );
    std::regex MADX::rgx_hdr_( "^\\@ (\\w+) +\\%([0-9]+s|le) +\\\"?([^\"\\n]+)" );
    std::regex MADX::rgx_elm_hdr_( "^\\s{0,}([\\*\\$])(.+)" );

    MADX::MADX( const char* filename, const char* ip_name, int direction, float max_s ) :
      ip_name_( ip_name ), dir_( direction/abs( direction ) ), beamline_( 0 ),
      s_offset_( 0. ), found_interaction_point_( false ),
      has_next_element_( false )
    {
      in_file_ = std::ifstream( filename );

      try {
        parseHeader();

        beamline_ = new Beamline( max_s );
        if ( max_s<0. and header_float_.hasKey( "length" ) ) beamline_->setLength( header_float_.get( "length" ) );
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
        parseElements();

        beamline_->computeSequence();

      } catch ( Exception& e ) { e.dump(); }
    }

    MADX::~MADX()
    {
      if ( in_file_.is_open() ) in_file_.close();
      if ( beamline_ ) delete beamline_;
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
        try { parseElement( values ); } catch ( Exception& e ) {
          if ( e.type()==Info ) break; // finished to parse
          e.dump();
        }
      }
    }

    void
    MADX::parseElement( const ValuesCollection& values )
    {
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
      const float s_bl = elem_map_floats.get( "s" ),
                  length = elem_map_floats.get( "l" );

      // start filling the beamline from the declared interaction point
      if ( !found_interaction_point_ ) {
        if ( name!=ip_name_ ) return;

        found_interaction_point_ = true;
        s_offset_ = s_bl;
      }

      const float s = s_bl-s_offset_-length;

      if ( s>beamline_->maxLength() ) {
        if ( has_next_element_ ) throw Exception( __PRETTY_FUNCTION__, "Finished to parse the beamline", Info );
        has_next_element_ = true;
      }

      // convert the element type from string to object
      const std::string elem_type = lowercase( trim( elem_map_str.get( "keyword" ) ) );
      const Element::Type elemtype = ElementDictionary::get().elementType( elem_type );

      try {
        // create the element
        switch ( elemtype ) {
          case Element::aGenericQuadrupole: {

            const float k1l = elem_map_floats.get( "k1l" ),
                        mag_str_k = -k1l/length;
            if ( k1l>0 ) { beamline_->addElement( new Element::HorizontalQuadrupole( name, s, length, mag_str_k ), true ); }
            else         { beamline_->addElement( new Element::VerticalQuadrupole( name, s, length, mag_str_k ), true ); }
          } break;
          case Element::aRectangularDipole: {
            const float k0l = elem_map_floats.get( "k0l" );
            if ( k0l==0. ) throw Exception( __PRETTY_FUNCTION__, Form( "Trying to add a rectangular dipole (%s) with k0l=%.2e", name.c_str(), k0l ), JustWarning );

            const float mag_strength = dir_*k0l/length;
            beamline_->addElement( new Element::RectangularDipole( name, s, length, mag_strength ), true );
          } break;
          case Element::aSectorDipole: {
            const float k0l = elem_map_floats.get( "k0l" );
            if ( k0l==0. ) throw Exception( __PRETTY_FUNCTION__, Form( "Trying to add a sector dipole (%s) with k0l=%.2e", name.c_str(), k0l ), JustWarning );

            const float mag_strength = dir_*k0l/length;
            beamline_->addElement( new Element::SectorDipole( name, s, length, mag_strength ), true );
          } break;
          case Element::anHorizontalKicker: {
            const float hkick = elem_map_floats.get( "hkick" );
            if ( hkick==0. ) throw Exception( __PRETTY_FUNCTION__, Form( "Trying to add a horizontal kicker (%s) with kick=%.2e", name.c_str(), hkick ), JustWarning );

            beamline_->addElement( new Element::HorizontalKicker( name, s, length, hkick ), true );
          } break;
          case Element::aVerticalKicker: {
            const float vkick = elem_map_floats.get( "vkick" );
            if ( vkick==0. ) throw Exception( __PRETTY_FUNCTION__, Form( "Trying to add a vertical kicker (%s) with kick=%.2e", name.c_str(), vkick ), JustWarning );

            beamline_->addElement( new Element::VerticalKicker( name, s, length, vkick ), true );
          } break;
          case Element::aRectangularCollimator: { beamline_->addElement( new Element::RectangularCollimator( name, s, length ), true ); } break;
          case Element::aMarker: {
            const Element::Marker marker( name, s, length );
            if ( name==ip_name_ ) { beamline_->addElement( &marker ); }
            //beamline_->addMarker( marker );
          } break;
          case Element::aMonitor:
          case Element::anInstrument: {
            beamline_->addMarker( Element::Marker( name, s, length ) );
          } break;
          case Element::aDrift: {
            previous_relpos_ = CLHEP::Hep2Vector( elem_map_floats.get( "x" ), elem_map_floats.get( "y" ) );
            previous_disp_ = CLHEP::Hep2Vector( elem_map_floats.get( "dx" ), elem_map_floats.get( "dy" ) );
            previous_beta_ = CLHEP::Hep2Vector( elem_map_floats.get( "betx" ), elem_map_floats.get( "bety" ) );
            has_next_element_ = false;
            return;
          } break;
          default: { has_next_element_ = false; } break;
        }

        // retrieve the pointer to the newly created beamline element
        Element::ElementBase* elem = beamline_->getElement( name );
        // check if beamline element was properly inserted
        if ( !elem ) return;

        const CLHEP::Hep2Vector relpos( elem_map_floats.get( "x" ), elem_map_floats.get( "y" ) );
        const int direction = +1; //FIXME
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

        { // associate the aperture type to the element
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
        }

      } catch ( Exception& e ) {
        e.dump();
        has_next_element_ = false;
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
}

