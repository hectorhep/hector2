#ifndef Elements_Aperture_h
#define Elements_Aperture_h

#include <CLHEP/Vector/TwoVector.h>

namespace Element
{
  class ApertureBase
  {
    public:
      typedef enum { None, Rectangular, Elliptic, Circular, RectElliptic } Type;
      friend std::ostream& operator<<( std::ostream&, const Type& );

    public:
      ApertureBase( const Type&, const CLHEP::Hep2Vector&, float, float, float, float );
      virtual ~ApertureBase();

      ApertureBase* clone() const;

      friend std::ostream& operator<<( std::ostream&, const ApertureBase& );
      friend std::ostream& operator<<( std::ostream&, const ApertureBase* );

      Type type() const { return type_; }
      virtual bool contains( const CLHEP::Hep2Vector& ) const;

      float x1() const { return x1_; }
      float x2() const { return x2_; }
      float x3() const { return x3_; }
      float x4() const { return x4_; }

      void setPosition( const CLHEP::Hep2Vector& pos ) { pos_ = pos; }
      void setPosition( float x, float y ) { setPosition( CLHEP::Hep2Vector( x, y ) ); }
      CLHEP::Hep2Vector position() const { return pos_; }

      float x() const { return pos_.x(); }
      float y() const { return pos_.y(); }

    protected:
      Type type_;
      CLHEP::Hep2Vector pos_;
      float x1_, x2_, x3_, x4_;
  };
}

#endif
