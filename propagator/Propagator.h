#ifndef Propagator_Propagator_h
#define Propagator_Propagator_h

#include "beamline/Beamline.h"
#include "beamline/Particle.h"

class Propagator
{
  public:
    Propagator( const Beamline* );
    ~Propagator();

    void propagate( Particle&, float );

  private:
    const Beamline* beamline_;
};

#endif
