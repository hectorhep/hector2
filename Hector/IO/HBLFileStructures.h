#ifndef Hector_IO_HBLFileStructures_h
#define Hector_IO_HBLFileStructures_h

namespace Hector
{
  namespace IO
  {
    struct HBLHeader
    {
      unsigned long long magic;
      unsigned short version;
      unsigned short num_elements;
    };
    struct HBLRunParameters
    {
    };
    struct HBLElement
    {
      HBLElement() :
        element_type( -1 ), element_name{ 0 }, element_s( 0. ), element_length( 0. ),
        element_magnetic_strength( 0. ),
        aperture_type( -1 ),
        aperture_p1( 0. ), aperture_p2( 0. ), aperture_p3( 0. ), aperture_p4( 0. ),
        aperture_x( 0. ), aperture_y( 0. ) {}
      HBLElement( const HBLElement& rhs ) :
        element_type( rhs.element_type ), element_s( rhs.element_s ), element_length( rhs.element_length ),
        element_magnetic_strength( rhs.element_magnetic_strength ),
        aperture_type( rhs.aperture_type ),
        aperture_p1( rhs.aperture_p1 ), aperture_p2( rhs.aperture_p1 ), aperture_p3( rhs.aperture_p3 ), aperture_p4( rhs.aperture_p4 ),
        aperture_x( rhs.aperture_x ), aperture_y( rhs.aperture_y ) {
        std::copy( rhs.element_name, rhs.element_name+20, element_name );
      }
      short element_type;
      char element_name[20];
      double element_s, element_length, element_magnetic_strength;
      short aperture_type;
      double aperture_p1, aperture_p2, aperture_p3, aperture_p4;
      double aperture_x, aperture_y;
    };
  }
}

#endif

