#ifndef Beamline_Beamline_h
#define Beamline_Beamline_h

#include "core/Exception.h"
#include "elements/ElementBase.h"
#include "Particle.h"

#include <CLHEP/Vector/ThreeVector.h>
#include <map>

using std::cout;

namespace Hector
{
  class Beamline
  {
    public:
      Beamline();
      Beamline( float, const CLHEP::Hep3Vector& ip=CLHEP::Hep3Vector() );
      ~Beamline();

      void clear();
      void dump( std::ostream& os=std::cout );

      CLHEP::Hep3Vector interactionPoint() const { return ip_; }

      void addElement( const Element::ElementBase*, bool delete_after=false );
      Element::ElementBase* getElement( const std::string& );
      Element::ElementBase* getElement( const char* name ) { return getElement( std::string( name ) ); }

      typedef std::vector<Element::ElementBase*> ElementsMap;
      const ElementsMap::const_iterator begin() const { return elements_.begin(); }
      const ElementsMap::const_iterator end() const { return elements_.end(); }

      void setLength( float length ) { length_ = length; }
      float length() const { return length_; }

      CLHEP::HepMatrix matrix( float, float, int );
      void propagate( const Particle&, float );

    private:
      float length_;
      CLHEP::Hep3Vector ip_;

      ElementsMap elements_;
  };
}

#endif
