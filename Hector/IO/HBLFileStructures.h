#ifndef Hector_IO_HBLFileStructures_h
#define Hector_IO_HBLFileStructures_h

namespace Hector
{
  namespace IO
  {
    /// Common header to HBL files
    /// \version 1.0.0
    struct HBLHeader
    {
      /// HBL file magic number (should be 0x464c4248 = 'HBL')
      unsigned long long magic;
      /// HBL file version
      unsigned short version;
      /// Number of beamline elements stored in HBL file
      /// \note Used in file corruption checks
      unsigned short num_elements;
    };
    /// A set of run parameters as stored in HBL files
    struct HBLRunParameters
    {
    };
    /// An element as stored in HBL files
    struct HBLElement
    {
      /// HBL element default constructor
      HBLElement() :
        element_type( -1 ), element_name( "\0" ),
        element_s( 0. ), element_length( 0. ),
        element_magnetic_strength( 0. ),
        aperture_type( -1 ),
        aperture_p1( 0. ), aperture_p2( 0. ), aperture_p3( 0. ), aperture_p4( 0. ),
        aperture_x( 0. ), aperture_y( 0. ) {}
      /// HBL element copy constructor
      HBLElement( const HBLElement& rhs ) :
        element_type( rhs.element_type ),
        element_s( rhs.element_s ), element_length( rhs.element_length ),
        element_magnetic_strength( rhs.element_magnetic_strength ),
        aperture_type( rhs.aperture_type ),
        aperture_p1( rhs.aperture_p1 ), aperture_p2( rhs.aperture_p1 ), aperture_p3( rhs.aperture_p3 ), aperture_p4( rhs.aperture_p4 ),
        aperture_x( rhs.aperture_x ), aperture_y( rhs.aperture_y ) {
        std::copy( rhs.element_name, rhs.element_name+sizeof( rhs.element_name ), element_name );
      }
      /// Enumerator handling the beamline element type
      short element_type;
      /// Beamline element name
      char element_name[50];
      /// Beamline element \f$s\f$ position
      double element_s;
      /// Beamline element length
      double element_length;
      /// Beamline element magnetic strength
      double element_magnetic_strength;
      /// Enumerator handling the beamline element aperture type
      short aperture_type;
      /// Aperture first shape parameter
      double aperture_p1;
      /// Aperture second shape parameter
      double aperture_p2;
      /// Aperture third shape parameter
      double aperture_p3;
      /// Aperture fourth shape parameter
      double aperture_p4;
      /// Aperture middle horizontal position
      double aperture_x;
      /// Aperture middle vertical position
      double aperture_y;
    };
  }
}

#endif

