#ifndef Elements_ElementBase_h
#define Elements_ElementBase_h

#include "ApertureBase.h"
#include <CLHEP/Vector/TwoVector.h>
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

      bool operator<( const ElementBase& rhs ) const { return s()<rhs.s(); }
      bool operator<( const ElementBase* rhs ) const { return s()<rhs->s(); }

      const char* name() const { return name_; }
      Type type() const { return type_; }

      void setS( float s ) { s_ = s; }
      /// Longitudinal position
      float s() const { return s_; }

      void setPosition( const CLHEP::Hep2Vector& pos ) { pos_ = pos; }
      CLHEP::Hep2Vector position() const { return pos_; }
      /// Horizontal position
      float x() const { return pos_.x(); }
      /// Vertical position
      float y() const { return pos_.y(); }

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

      void setAperture( const ApertureBase* apert ) { aperture_ = apert->clone(); }
      ApertureBase* aperture() { return aperture_; }

    protected:
      Type type_;
      const char* name_;
      ApertureBase* aperture_;

      float length_;
      float magnetic_strength_;

      CLHEP::Hep2Vector pos_;
      float s_;

      float tx_, ty_;

      CLHEP::HepMatrix matrix_;
  };

}

#endif
