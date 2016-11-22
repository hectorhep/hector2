#ifndef Elements_Quadrupole_h
#define Elements_Quadrupole_h

#include "ElementBase.h"

namespace Element
{
  class Quadrupole : public ElementBase
  {
    public:
      Quadrupole( const std::string& name ) : ElementBase( ElementBase::Quadrupole, name ) {;}

      virtual Quadrupole* clone() const = 0;
      virtual CLHEP::HepMatrix matrix( float, float, int ) const = 0;

    protected:
  };

  class HorizontalQuadrupole : public Quadrupole
  {
    public:
      HorizontalQuadrupole( const std::string& name ) : Quadrupole( name ) {;}

      HorizontalQuadrupole* clone() const { return new HorizontalQuadrupole( *this ); }
      CLHEP::HepMatrix matrix( float, float, int ) const;

    private:
  };

  class VerticalQuadrupole : public Quadrupole
  {
    public:
      VerticalQuadrupole( const std::string& name ) : Quadrupole( name ) {;}

      VerticalQuadrupole* clone() const { return new VerticalQuadrupole( *this ); }
      CLHEP::HepMatrix matrix( float, float, int ) const;

    private:
  };
}

#endif
