#ifndef Elements_ElementBase_h
#define Elements_ElementBase_h

#include <CLHEP/Matrix/Matrix.h>

namespace Element
{
  class ElementBase
  {
    public:
      typedef enum {
        Invalid = -1,
        Marker, Drift, Monitor,
        RectangularDipole, SectorDipole,
        Quadrupole, Sextupole, Multipole,
        //VerticalQuadrupole, HorizontalQuadrupole,
        VerticalKicker, HorizontalKicker,
        //RectangularCollimator, EllipticalCollimator, CylindricalCollimator,
        //RomanPot, InteractionPoint,
        Placeholder, Instrument
      } Type;

    public:
      ElementBase( const Type&, const char* name="invalid element" );
      virtual ~ElementBase();

      ElementBase* clone() const;

      bool operator<( const ElementBase& rhs ) const { return s_<rhs.s(); }
      bool operator<( const ElementBase* rhs ) const { return s_<rhs->s(); }
      /*struct sorter {
        inline bool operator()( const ElementBase& el_1, const ElementBase& el_2 ) const { return ( el_1.s()<el_2.s() ); }
        inline bool operator()( const ElementBase* el_1, const ElementBase* el_2 ) const { return ( el_1->s()<el_2->s() ); }
      } ordering;*/


      const char* name() const { return name_; }
      Type type() const { return type_; }

      void setS( float s ) { s_ = s; }
      /// Longitudinal position
      float s() const { return s_; }

      void setX( float x ) { xpos_ = x; }
      /// Horizontal position
      float x() const { return xpos_; }

      void setY( float y ) { ypos_ = y; }
      /// Vertical position
      float y() const { return ypos_; }

      void setTx( float tx ) { tx_ = tx; }
      // Horizontal angle
      float Tx() const { return tx_; }
      void setTy( float ty ) { ty_ = ty; }
      // Vertical angle
      float Ty() const { return ty_; }

      void setLength( float length ) { length_ = length; }
      /// Element length
      float length() const { return length_; }

      void setMagneticStrength( float k ) { magnetic_strength_ = k; }
      /// Magnetic field strength
      float magneticStrength() const { return magnetic_strength_; }

      CLHEP::HepMatrix matrix() const { return matrix_; }

    protected:
      Type type_;
      const char* name_;

      float s_;
      float length_;
      float magnetic_strength_;
      float xpos_, ypos_;
      float tx_, ty_;

      CLHEP::HepMatrix matrix_;
  };

}

#endif
