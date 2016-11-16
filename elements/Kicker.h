#ifndef Elements_Kicker_h
#define Elements_Kicker_h

#include "ElementBase.h"

namespace Element
{
  class Kicker : public ElementBase
  {
    public:
      Kicker( const ElementBase::Type& type, const std::string& name ) : ElementBase( type, name ) {;}
      ~Kicker() {;}
  };

  class HorizontalKicker : public Kicker
  {
    public:
      HorizontalKicker( const std::string& name ) : Kicker( ElementBase::HorizontalKicker, name ) {;}
      ~HorizontalKicker() {;}
  };

  class VerticalKicker : public Kicker
  {
    public:
      VerticalKicker( const std::string& name ) : Kicker( ElementBase::VerticalKicker, name ) {;}
      ~VerticalKicker() {;}
  };
}

#endif
