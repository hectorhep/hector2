#include "Beamline.h"

namespace Hector
{
  Beamline::Beamline() :
    max_length_( 0. ), drifts_added_( false )
  {
  }

  Beamline::Beamline( const Beamline& rhs, bool copy_elements ) :
    max_length_( rhs.max_length_ ), ip_( rhs.ip_ ),
    markers_( rhs.markers_ ),
    drifts_added_( rhs.drifts_added_ )
  {
    clear();
    if ( copy_elements ) setElements( rhs, false );
  }

  Beamline::Beamline( float length, const CLHEP::Hep3Vector& ip ) :
    max_length_( length+5. ), // artificially increase the size to include next elements
    ip_( ip ), drifts_added_( false )
  {
  }


  Beamline::~Beamline()
  {
    clear();
    markers_.clear();
  }

  void
  Beamline::clear()
  {
    for ( ElementsMap::iterator elem=elements_.begin(); elem!=elements_.end(); elem++ ) {
      if ( *elem ) delete ( *elem );
    }
    elements_.clear();
  }

  void
  Beamline::addMarker( const Element::Marker& marker )
  {
    markers_.insert( std::pair<float,Element::Marker>( marker.s(), marker ) );
  }

  void
  Beamline::addElement( const Element::ElementBase* elem, bool delete_after )
  {
    const float new_size = elem->s()+elem->length();
    if ( new_size>max_length_ and max_length_<0. ) {
      if ( delete_after ) delete elem;
      throw Exception( __PRETTY_FUNCTION__, Form( "Element %s is too far away for this beamline!\n"
                                                  "\tBeamline length: %.3f m, this element: %.3f m",
                                                  elem->name().c_str(), max_length_, new_size ), Fatal );
    }

    bool already_added = false;

    // check the overlaps before adding
    for ( ElementsMap::iterator it=elements_.begin(); it!=elements_.end(); it++ ) {
      Element::ElementBase* prev_elem = *( it );

      // first check if the element is already present in the beamline
      if ( *prev_elem==*elem ) { already_added = true; break; }

      if ( prev_elem->s()>elem->s() ) break;
      if ( prev_elem->s()+prev_elem->length()<=elem->s() ) continue;
      if ( prev_elem->length()==0 ) continue;

      if ( !Parameters::correct_beamline_overlaps ) {
        throw Exception( __PRETTY_FUNCTION__, Form( "Elements overlap with \"%s\" detected while adding \"%s\"!", prev_elem->name().c_str(), elem->name().c_str() ), Fatal );
      }

      // from that point on, an overlap is detected
      // reduce or separate that element in two sub-parts

      std::ostringstream os_elem; os_elem << elem->type();
      std::ostringstream os_prevelem; os_prevelem << prev_elem->type();

      PrintWarning( Form( "%s (%s) is inside %s (%s)\n\tHector will fix the overlap by splitting the earlier.",
                          elem->name().c_str(), os_elem.str().c_str(), prev_elem->name().c_str(), os_prevelem.str().c_str() ) );
      const float prev_length = prev_elem->length();
      elements_.push_back( elem->clone() );
      already_added = true;

      // check if one needs to add an extra piece to the previous element
      if ( elem->s()+elem->length()<prev_elem->s()+prev_elem->length() ) {
        const std::string prev_name = prev_elem->name();
        // add an extra sub-part (leftover from the previous element)
        prev_elem->setName( Form( "%s.part1", prev_name.c_str() ) );
        Element::ElementBase* next_elem = prev_elem->clone();
        next_elem->setName( Form( "%s.part2", prev_name.c_str() ) );
        next_elem->setS( elem->s()+elem->length() );
        next_elem->setLength( prev_length-elem->length() );
        elements_.push_back( next_elem );
      }
      prev_elem->setLength( elem->s()-prev_elem->s() );
      break;
    }

    if ( !already_added ) {
      elements_.push_back( elem->clone() );
    }
    // clean the memory if needed
    if ( delete_after ) delete elem;

    // sort all beamline elements according to their s-position
    std::sort( elements_.begin(), elements_.end(), Element::ElementsSorter() );

    // enable a new computing of the drift elements
    drifts_added_ = false;
  }

  Element::ElementBase*
  Beamline::getElement( const std::string& name )
  {
    for ( size_t i=0; i<elements_.size(); i++ ) {
      Element::ElementBase* elem = elements_.at( i );
      if ( elem->name().find( name )!=std::string::npos ) return elem;
    }
    return 0;
  }

  Element::ElementBase*
  Beamline::getElement( float s )
  {
    for ( size_t i=0; i<elements_.size(); i++ ) {
      Element::ElementBase* elem = elements_.at( i );
      if ( elem->s()>s ) continue;
      if ( elem->s()<=s and elem->s()+elem->length()>=s ) return elem;
    }
    return 0;
  }

  CLHEP::HepMatrix
  Beamline::matrix( float eloss, float mp, int qp )
  {
    CLHEP::HepMatrix out = CLHEP::HepDiagMatrix( 6, 1 );

    for ( size_t i=0; i<elements_.size(); i++ ) {
      Element::ElementBase* elem = elements_.at( i );
      out = out * elem->matrix( eloss, mp, qp );
    }

    return out;
  }

  float
  Beamline::length() const
  {
    if ( elements_.size()==0 ) return 0.;
    Element::ElementBase* elem = *elements_.rbegin();
    return ( elem->s() + elem->length() );
  }

  void
  Beamline::dump( std::ostream& os )
  {
    os << "=============== Beamline dump ===============\n"
       << " interaction point: " << interactionPoint() << "\n"
       << " length: " << length() << " m\n"
       << " elements list: ";
    for ( ElementsMap::const_iterator it=elements_.begin(); it!=elements_.end(); it++ ) {
      os << "\n  * " << *it;
    }
    os << "\n";
  }

  void
  Beamline::offsetElementsAfter( float s, const CLHEP::Hep2Vector& offset )
  {
    for ( ElementsMap::iterator it=elements_.begin(); it!=elements_.end(); it++ ) {
      Element::ElementBase* elem = *( it );
      if ( elem->s()<s ) continue;
      elem->offset( offset );
    }
  }

  void
  Beamline::tiltElementsAfter( float s, const CLHEP::Hep2Vector& tilt )
  {
    for ( ElementsMap::iterator it=elements_.begin(); it!=elements_.end(); it++ ) {
      Element::ElementBase* elem = *( it );
      if ( elem->s()<s ) continue;
      elem->tilt( tilt );
    }
  }

  void
  Beamline::computeSequence()
  {
    // only compute the sequence once
    if ( drifts_added_ ) return;

    // add the drifts between optical elements
    float pos = 0.;
    // brand new beamline to populate
    Beamline tmp( *this, false );

    // convert all empty spaces into drifts
    for ( ElementsMap::const_iterator it=elements_.begin(); it!=elements_.end(); it++ ) {
      const Element::ElementBase* elem = *( it );
      // skip the markers
      if ( it!=elements_.begin() and elem->type()==Element::aMarker and elem->s()!=ip_.z() ) continue;
      // add a drift whenever there is a gap in s
      const float drift_length = elem->s()-pos;
      if ( drift_length>0. ) {
        try {
          tmp.addElement( new Element::Drift( Form( "drift:%.4E", pos ).c_str(), pos, drift_length ), true );
        } catch ( Exception& e ) { e.dump(); }
      }
      try { tmp.addElement( elem ); } catch ( Exception& e ) { e.dump(); }
      pos = elem->s()+elem->length();
    }
    // add the last drift
    const float drift_length = tmp.length()-pos;
    if ( drift_length>0 ) {
      try {
        tmp.addElement( new Element::Drift( Form( "drift:%.4E", pos ).c_str(), pos, drift_length ), true );
      } catch ( Exception& e ) { e.dump(); }
    }

    // replace the current beamline content with the sequenced one
    clear();
    setElements( tmp, true );
    tmp.dump();

    // make sure that the sequence cannot be computed again
    drifts_added_ = true;
  }

  void
  Beamline::setElements( const Beamline& moth_bl, bool delete_after )
  {
    for ( ElementsMap::const_iterator it=moth_bl.begin(); it!=moth_bl.end(); it++ ) {
      addElement( ( *( it ) )->clone(), delete_after );
    }
  }
}
