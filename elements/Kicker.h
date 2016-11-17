#ifndef Elements_Kicker_h
#define Elements_Kicker_h

#include "ElementBase.h"
#include "core/Constants.h"

namespace Element
{
  class Kicker : public ElementBase
  {
    public:
      Kicker( const ElementBase::Type& type, const std::string& name ) : ElementBase( type, name ) {;}
      ~Kicker() {;}

      virtual Kicker* clone() const = 0;
      virtual void computeMatrix( float, float, int ) = 0;
  };

  class HorizontalKicker : public Kicker
  {
    public:
      HorizontalKicker( const std::string& name ) : Kicker( ElementBase::HorizontalKicker, name ) {;}
      ~HorizontalKicker() {;}

      HorizontalKicker* clone() const { return new HorizontalKicker( *this ); }
      void computeMatrix( float, float, int );
  };

  class VerticalKicker : public Kicker
  {
    public:
      VerticalKicker( const std::string& name ) : Kicker( ElementBase::VerticalKicker, name ) {;}
      ~VerticalKicker() {;}

      VerticalKicker* clone() const { return new VerticalKicker( *this ); }
      void computeMatrix( float, float, int );
  };
}

#endif
