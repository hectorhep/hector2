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
      ParticleStoppedException( const char* from, ExceptionType type = Undefined, const Element::ElementBase* elem = nullptr ) :
        Exception( from, type, 10000 ), elem_( elem ) {
        message_ << "Particle stopped";
        if ( elem )
          message_ << " at " << elem->name() << " (" << elem->type() << ")";
        message_ << ".\n";
      }
      /// Retrieve the beamline element that stopped the particle
      const Element::ElementBase* stoppingElement() const { return elem_; }

    private:
      /// Beamline element that stopped the particle
      const Element::ElementBase* elem_;
  };
}

#endif

