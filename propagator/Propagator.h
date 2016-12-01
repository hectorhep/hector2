#ifndef Propagator_Propagator_h
#define Propagator_Propagator_h

#include "beamline/Beamline.h"
#include "beamline/Particle.h"

namespace Hector
{
  class Propagator
  {
    public:
      Propagator( const Beamline* );
      ~Propagator();

      void propagate( Particle&, float ) const;
      std::pair<float,Particle::StateVector> hitPosition( const Particle::StateVector& ini_pos, const Element::ElementBase* ele, float eloss, float mp, int qp ) const;

    private:
      const Beamline* beamline_;
  };
}

#endif
