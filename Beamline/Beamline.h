#ifndef Hector_Beamline_Beamline_h
#define Hector_Beamline_Beamline_h

#include "Core/Exception.h"
#include "Elements/ElementBase.h"
#include "Elements/Drift.h"
#include "Elements/Marker.h"
#include "Propagator/Particle.h"

#include <CLHEP/Vector/ThreeVector.h>
#include <map>

using std::cout;

namespace Hector
{
  /// A beamline, or collection of optics elements
  class Beamline
  {
    public:
      /// List of markers in the beamline
      typedef std::map<float,Element::Marker> MarkersMap;

    public:
      Beamline();
      /// Copy constructor
      Beamline( const Beamline&, bool copy_elements=true );
      /// Build a beamline from a longitudinal size and a interaction point position
      /// \param[in] length Longitudinal length of the beamline
      /// \param[in] ip Position of the interaction point
      Beamline( float length, const CLHEP::Hep3Vector& ip=CLHEP::Hep3Vector() );
      ~Beamline();

      /// Compute all drifts between each element in the beamline
      static Beamline* sequencedBeamline( const Beamline* );

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
      const Elements& elements() const { return elements_; }
      /// Retrieve a beamline element given its name
      /// \param[in] name Name of the element to be retrieved
      const Element::ElementBase* getElement( const std::string& name ) const;
      /// Retrieve a beamline element given its name
      /// \param[in] name Name of the element to be retrieved
      const Element::ElementBase* getElement( const char* name ) const { return getElement( std::string( name ) ); }
      /// Retrieve a beamline element given its s-position
      /// \param[in] s s-position of the element (computed wrt the interaction point)
      const Element::ElementBase* getElement( float s ) const;
      /// Number of elements in the beamline
      unsigned short numElements() const { return elements_.size(); }

      /// Iterator to the first element in the beamline
      Elements::iterator begin() { return elements_.begin(); }
      /// Iterator to the last element in the beamline
      Elements::iterator end() { return elements_.end(); }
      /// Constant iterator to the first element in the beamline
      const Elements::const_iterator begin() const { return elements_.begin(); }
      /// Constant iterator to the last element in the beamline
      const Elements::const_iterator end() const { return elements_.end(); }

      /// Add a new marker element to the collection
      void addMarker( const Element::Marker& marker );
      /// Constant iterator to the first marker in the beamline
      const MarkersMap::const_iterator markers_begin() const { return markers_.begin(); }
      /// Constant iterator to the last marker in the beamline
      const MarkersMap::const_iterator markers_end() const { return markers_.end(); }

      /// Set the longitudinal length of the beamline (in m)
      void setLength( float length ) { max_length_ = length; }
      /// Longitudinal length of the beamline (in m)
      float length() const;
      /// Maximal length of the beamline (in m)
      float maxLength() const { return max_length_; }

      /// Offset all elements after a given s-coordinate
      void offsetElementsAfter( float s, const CLHEP::Hep2Vector& offset );
      /// Tilt all elements after a given s-coordinate
      void tiltElementsAfter( float s, const CLHEP::Hep2Vector& offset );

      /// Total propagation matrix of all combined beamline elements
      CLHEP::HepMatrix matrix( float, float, int );

    private:
      /// Copy the list of elements from one beamline to this one
      void setElements( const Beamline& moth_bl, bool delete_after=false );
      /// Beamline maximal length (in m)
      float max_length_;
      /// Position of the interaction point
      CLHEP::Hep3Vector ip_;

      /// List of elements defining the beamline
      Elements elements_;
      /// List of markers in the beamline
      MarkersMap markers_;
  };
}

#endif
