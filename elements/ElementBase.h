#ifndef Elements_ElementBase_h
#define Elements_ElementBase_h

#include "ApertureBase.h"
#include "core/Utils.h"
#include "core/Constants.h"

#include <sstream>

#include <CLHEP/Vector/TwoVector.h>
#include <CLHEP/Matrix/Matrix.h>
#include <CLHEP/Matrix/DiagMatrix.h>

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
        VerticalKicker, HorizontalKicker,
        RectangularCollimator, EllipticalCollimator, CircularCollimator,
        //RomanPot, InteractionPoint,
        Placeholder, Instrument
      } Type;
      friend std::ostream& operator<<( std::ostream&, const Type& );

    public:
      ElementBase( const Type&, const std::string& name="invalid element" );
      virtual ~ElementBase();

      virtual ElementBase* clone() const = 0;
      virtual void computeMatrix( float, float, int ) = 0;

      friend std::ostream& operator<<( std::ostream&, const ElementBase& );
      friend std::ostream& operator<<( std::ostream&, const ElementBase* );

      const std::string& name() const { return name_; }
      Type type() const { return type_; }

      void setS( float s ) { s_ = s; }
      /// Longitudinal position
      float s() const { return s_; }

      void setPosition( const CLHEP::Hep2Vector& pos ) { pos_ = pos; }
      /// X-Y position of the element at a given s
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

      void setBeta( const CLHEP::Hep2Vector& beta ) { beta_ = beta; }
      CLHEP::Hep2Vector beta() const { return beta_; }

      void setDispersion( const CLHEP::Hep2Vector& disp ) { disp_ = disp; }
      /// Horizontal and vertical dispersion
      CLHEP::Hep2Vector dispersion() const { return disp_; }

      void setRelativePosition( const CLHEP::Hep2Vector& pos ) { rel_pos_ = pos; };
      CLHEP::Hep2Vector relativePosition() const { return rel_pos_; }

      CLHEP::HepMatrix matrix() const { return matrix_; }

      void setAperture( const ApertureBase* apert ) { aperture_ = apert->clone(); }
      ApertureBase* aperture() const { return aperture_; }

      float fieldStrength( float, float, int ) const;

    protected:
      Type type_;
      const std::string name_;
      ApertureBase* aperture_;

      float length_;
      float magnetic_strength_;

      CLHEP::Hep2Vector pos_;
      float s_;

      float tx_, ty_;

      CLHEP::Hep2Vector beta_;
      CLHEP::Hep2Vector disp_;
      CLHEP::Hep2Vector rel_pos_;

      CLHEP::HepMatrix matrix_;
  };

  struct sorter
  {
    inline bool operator()( const ElementBase& lhs, const ElementBase& rhs ) { return ( lhs.s()<rhs.s() ); }
    inline bool operator()( const ElementBase* lhs, const ElementBase* rhs ) { return ( lhs->s()<rhs->s() ); }
  };

}

#endif
