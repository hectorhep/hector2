#ifndef Beamline_Beamline_h
#define Beamline_Beamline_h

#include "core/Exception.h"
#include "elements/ElementBase.h"
#include "Particle.h"

#include <CLHEP/Vector/ThreeVector.h>
#include <map>

class Beamline
{
  public:
    Beamline();
    ~Beamline();

    CLHEP::Hep3Vector interactionPoint() const { return ip_; }

    void addElement( const Element::ElementBase* );
    Element::ElementBase* getElement( const std::string& );
    Element::ElementBase* getElement( const char* name ) { return getElement( std::string( name ) ); }

    typedef std::vector<Element::ElementBase*> ElementsMap;
    ElementsMap::const_iterator begin() { return elements_.begin(); }
    ElementsMap::const_iterator end() { return elements_.end(); }

    void setLength( float length ) { length_ = length; }
    float length() const { return length_; }

    CLHEP::HepMatrix matrix( float, float, int );
    void propagate( const Particle&, float );

  private:
    CLHEP::Hep3Vector ip_;
    float length_;

    ElementsMap elements_;
};

#endif
