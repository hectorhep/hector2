#ifndef Beamline_Beamline_h
#define Beamline_Beamline_h

#include "core/Exception.h"
#include "elements/ElementBase.h"
#include "propagator/Particle.h"

#include <CLHEP/Vector/ThreeVector.h>
#include <map>

using std::cout;

namespace Hector
{
  class Beamline
  {
    public:
      Beamline();
      /// Build a beamline from a longitudinal size and a interaction point position
      Beamline( float, const CLHEP::Hep3Vector& ip=CLHEP::Hep3Vector() );
      ~Beamline();

      /// Remove and clean all elements in the beamline
      void clear();
      void dump( std::ostream& os=std::cout );

      /// Retrieve the position of the interaction point
      CLHEP::Hep3Vector interactionPoint() const { return ip_; }

      /// Add a new element in the beamline
      void addElement( const Element::ElementBase*, bool delete_after=false );
      /// Retrieve a beamline element given its name
      Element::ElementBase* getElement( const std::string& );
      /// Retrieve a beamline element given its name
      Element::ElementBase* getElement( const char* name ) { return getElement( std::string( name ) ); }

      /// List of elements in the beamline
      typedef std::vector<Element::ElementBase*> ElementsMap;
      /// Constant iterator to the first element in the beamline
      const ElementsMap::const_iterator begin() const { return elements_.begin(); }
      /// Constant iterator to the last element in the beamline
      const ElementsMap::const_iterator end() const { return elements_.end(); }

      /// Set the longitudinal length of the beamline (m)
      void setLength( float length ) { length_ = length; }
      /// Longitudinal length of the beamline (m)
      float length() const { return length_; }

      /// Total propagation matrix of all combined beamline elements
      CLHEP::HepMatrix matrix( float, float, int );
      void propagate( const Particle&, float );

    private:
      /// Beamline length (m)
      float length_;
      /// Position of the interaction point
      CLHEP::Hep3Vector ip_;

      /// List of elements defining the beamline
      ElementsMap elements_;
      bool has_next_element_;
  };
}

#endif
