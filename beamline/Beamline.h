#ifndef Beamline_Beamline_h
#define Beamline_Beamline_h

#include "core/Exception.h"
#include "elements/ElementBase.h"
#include "elements/Drift.h"
#include "propagator/Particle.h"

#include <CLHEP/Vector/ThreeVector.h>
#include <map>

using std::cout;

namespace Hector
{
  /// A beamline, or collection of optics elements
  class Beamline
  {
    public:
      /// List of elements in the beamline
      typedef std::vector<Element::ElementBase*> ElementsMap;

    public:
      Beamline();
      /// Copy constructor
      Beamline( const Beamline& );
      /// Build a beamline from a longitudinal size and a interaction point position
      /// \param[in] length Longitudinal length of the beamline
      /// \param[in] ip Position of the interaction point
      Beamline( float length, const CLHEP::Hep3Vector& ip=CLHEP::Hep3Vector() );
      ~Beamline();

      /// Remove and clean all elements in the beamline
      void clear();
      /// Print all useful information on a beamline and all its enclosing elements
      /// \param[out] os Output stream where to dump the information
      void dump( std::ostream& os=std::cout );

      /// Retrieve the position of the interaction point
      CLHEP::Hep3Vector interactionPoint() const { return ip_; }

      /// Add a new element in the beamline
      /// \param[in] elem Element to be copied and added to the beamline
      /// \param[in] delete_after Is the parent element to be deleted afterwards?
      void addElement( const Element::ElementBase* elem, bool delete_after=false );
      /// Get the full beamline content (vector of elements)
      const ElementsMap& elements() const { return elements_; }
      /// Retrieve a beamline element given its name
      /// \param[in] name Name of the element to be retrieved
      Element::ElementBase* getElement( const std::string& name );
      /// Retrieve a beamline element given its name
      /// \param[in] name Name of the element to be retrieved
      Element::ElementBase* getElement( const char* name ) { return getElement( std::string( name ) ); }
      /// Retrieve a beamline element given its s-position
      /// \param[in] s s-position of the element (computed wrt the interaction point)
      Element::ElementBase* getElement( float s );
      /// Number of elements in the beamline
      unsigned short numElements() const { return elements_.size(); }

      /// Constant iterator to the first element in the beamline
      const ElementsMap::const_iterator begin() const { return elements_.begin(); }
      /// Constant iterator to the last element in the beamline
      const ElementsMap::const_iterator end() const { return elements_.end(); }

      /// Set the longitudinal length of the beamline (in m)
      void setLength( float length ) { max_length_ = length; }
      /// Longitudinal length of the beamline (in m)
      float length() const;

      /// Offset all elements after a given s-coordinate
      void offsetElementsAfter( float s, const CLHEP::Hep2Vector& offset );
      /// Tilt all elements after a given s-coordinate
      void tiltElementsAfter( float s, const CLHEP::Hep2Vector& offset );

      /// Total propagation matrix of all combined beamline elements
      CLHEP::HepMatrix matrix( float, float, int );

      /// Compute all drifts between each element in the beamline
      void computeSequence();

    private:
      void setElements( const ElementsMap& elem_map );
      /// Beamline maximal length (in m)
      float max_length_;
      /// Position of the interaction point
      CLHEP::Hep3Vector ip_;

      /// List of elements defining the beamline
      ElementsMap elements_;
      bool has_next_element_;
      bool drifts_added_;
  };
}

#endif
