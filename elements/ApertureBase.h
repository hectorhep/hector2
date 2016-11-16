#ifndef Elements_Aperture_h
#define Elements_Aperture_h

namespace Element
{
  class ApertureBase
  {
    public:
      typedef enum { None, Rectangular, Elliptic, Circular, RectElliptic } Type;

    public:
      ApertureBase();
      ~ApertureBase();

      Type type() const { return type_; }
      virtual bool contains( float, float ) const=0;

    protected:
      Type type_;
      float x1, x2, x3, x4;
      float xpos_, ypos_;
  };
}

#endif
