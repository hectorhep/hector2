#ifndef Beamline_Beamline_h
#define Beamline_Beamline_h

#include "core/Exception.h"
#include "elements/ElementBase.h"

#include <CLHEP/Vector/ThreeVector.h>
#include <map>

class Beamline
{
  public:
    Beamline();
    ~Beamline();

    CLHEP::Hep3Vector interactionPoint() const { return ip_; }

    void addElement( const Element::ElementBase* );

    void setLength( float length ) { length_ = length; }
    float length() const { return length_; }

  private:
    CLHEP::Hep3Vector ip_;
    float length_;

    typedef std::vector<Element::ElementBase*> ElementsMap;
    ElementsMap elements_;
};

#endif
