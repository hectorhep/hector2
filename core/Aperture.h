#ifndef core_Aperture_h
#define core_Aperture_h

class Aperture
{
  public:
    typedef enum { None, Rectangular, Elliptic, Circular, RectEllipse } Type;

  public:
    Aperture();
    ~Aperture();

    Type type() const { return type_; }
    virtual bool contains( float, float ) const=0;

  protected:
    Type type_;
    float x1, x2, x3, x4
    float xpos_, ypos_;
};

#endif
