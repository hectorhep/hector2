#ifndef Hector_Core_ParticleStoppedException_h
#define Hector_Core_ParticleStoppedException_h

#include "Hector/Core/Exception.h"
#include "Hector/Elements/ElementBase.h"

namespace Hector
{
  class ParticleStoppedException : public Exception
  {
    public:
      ParticleStoppedException( const char* from, const Element::ElementBase* elem, ExceptionType type = Undefined, const char* additional_info = "" ) :
        Exception( from, Form( "Particle stopped at %s (%s).\n%s", elem->name().c_str(), elem->typeName().c_str(), additional_info ), type, 10000 ),
        elem_( elem ) {}
        const Element::ElementBase* stoppingElement() const { return elem_; }

    private:
      const Element::ElementBase* elem_;
  };
}

#endif

