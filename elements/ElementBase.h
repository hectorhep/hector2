#ifndef elements_ElementBase_h
#define elements_ElementBase_h

namespace Element
{
  class ElementBase
  {
    public:
      typedef enum {
        Drift, rDipole, sDipole,
        verticalQuadrupole, horizontalQuadrupole,
        verticalKicker, horizontalKicker,
        rCollimator, eCollimator, cCollimator,
        RP, IP, Marker
      } Type;


    public:
      ElementBase( const char* name="invalid element" );
      virtual ~ElementBase();

      const char* name() const { return name_; }
      Type type() const { return type_; }

      /// Longitudinal position
      float s() const { return s_; }
      /// Horizontal position
      float x() const { return xpos_; }
      /// Vertical position
      float y() const { return ypos_; }

      float Tx() const { return tx_; }
      float Ty() const { return ty_; }

      float length() const { return element_length_; }
      float magneticStrength() const { return magnetic_strength_; }

    protected:
      const char* name_;
      Type type_;

      float s_;
      float element_length_;
      float magnetic_strength_;
      float xpos_, ypos_;
      float tx_, ty_;
  };
}

#endif
