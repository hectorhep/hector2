#ifndef Elements_Kicker_h
#define Elements_Kicker_h

#include "ElementBase.h"
#include "core/Constants.h"

namespace Hector
{
  namespace Element
  {
    class Kicker : public ElementBase
    {
      public:
        Kicker( const ElementBase::Type& type, const std::string& name ) : ElementBase( type, name ) {;}

        virtual Kicker* clone() const = 0;
        virtual CLHEP::HepMatrix matrix( float, float, int ) const = 0;
    };

    class HorizontalKicker : public Kicker
    {
      public:
        HorizontalKicker( const std::string& name ) : Kicker( ElementBase::HorizontalKicker, name ) {;}

        HorizontalKicker* clone() const { return new HorizontalKicker( *this ); }
        CLHEP::HepMatrix matrix( float, float, int ) const;
    };

    class VerticalKicker : public Kicker
    {
      public:
        VerticalKicker( const std::string& name ) : Kicker( ElementBase::VerticalKicker, name ) {;}

        VerticalKicker* clone() const { return new VerticalKicker( *this ); }
        CLHEP::HepMatrix matrix( float, float, int ) const;
    };
  }
}

#endif
