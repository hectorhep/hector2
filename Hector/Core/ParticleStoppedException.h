#ifndef Hector_Core_ParticleStoppedException_h
#define Hector_Core_ParticleStoppedException_h

#include "Hector/Core/Exception.h"
#include "Hector/Elements/ElementBase.h"

namespace Hector
{
  /// Specific exception for particles stopped in the course of a beamline propagation
  class ParticleStoppedException : public Exception
  {
    public:
      /// Exception constructor
      /// \param[in] from Method raising the exception
      /// \param[in] elem Beamline element stopping the particle
      /// \param[in] type Exception type
      /// \param[in] additional_info Any other useful information to help the debugging
      ParticleStoppedException( const char* from, const Element::ElementBase* elem, ExceptionType type = Undefined, const char* additional_info = "" ) :
        Exception( from, Form( "Particle stopped at %s (%s).\n%s", elem->name().c_str(), elem->typeName().c_str(), additional_info ), type, 10000 ),
        elem_( elem ) {}
      /// Retrieve the beamline element that stopped the particle
      const Element::ElementBase* stoppingElement() const { return elem_; }

    private:
      /// Beamline element that stopped the particle
      const Element::ElementBase* elem_;
  };
}

#endif

