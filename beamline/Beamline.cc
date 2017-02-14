#include "Beamline.h"

namespace Hector
{
  Beamline::Beamline() :
    max_length_( 0. ), has_next_element_( false ), drifts_added_( false )
  {
  }

  Beamline::Beamline( const Beamline& rhs ) :
    max_length_( rhs.max_length_ ), ip_( rhs.ip_ ),
    has_next_element_( rhs.has_next_element_ ), drifts_added_( rhs.drifts_added_ )
  {
    clear();
    setElements( rhs.elements() );
  }

  Beamline::Beamline( float length, const CLHEP::Hep3Vector& ip ) :
    max_length_( length ), ip_( ip ), has_next_element_( false ), drifts_added_( false )
  {
  }


  Beamline::~Beamline()
  {
    clear();
  }

  void
  Beamline::clear()
  {
    for ( ElementsMap::iterator elem=elements_.begin(); elem!=elements_.end(); elem++ ) {
      if ( *elem ) delete *( elem );
    }
    elements_.clear();
  }

  void
  Beamline::addElement( const Element::ElementBase* elem, bool delete_after )
  {
    const float new_size = elem->s()+elem->length();
    if ( new_size>max_length_ ) {
      if ( has_next_element_ ) {
        if ( max_length_<0. ) {
          throw Exception( __PRETTY_FUNCTION__, Form( "Element %s is too far away for this beamline!\n"
                                                      "\tBeamline length: %.3f m, this element: %.3f m",
                                                      elem->name().c_str(), max_length_, new_size ), Fatal );
        }
        return;
      }
      else has_next_element_ = true;
    }

    elements_.push_back( elem->clone() );
    std::sort( elements_.begin(), elements_.end(), Element::ElementsSorter() );

    // clean the memory if needed
    if ( delete_after ) delete elem;

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
    Element::ElementBase* elem = *( --elements_.end() );
    return elem->s()+elem->length();
  }

  void
  Beamline::dump( std::ostream& os )
  {
    os << "========== Beamline dump ==========\n"
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
    Beamline tmp( *this );
    tmp.clear();
    for ( ElementsMap::const_iterator it=elements_.begin(); it!=elements_.end(); it++ ) {
      const Element::ElementBase* elem = *( it );
      if ( it!=elements_.begin() and elem->type()==Element::ElementBase::Marker ) continue;
      // add a drift whenever there is a gap in s
      const float drift_length = elem->s()-pos;
      if ( drift_length>0. ) {
        Element::Drift drift( Form( "drift:%.4E", pos ).c_str() );
        drift.setS( pos );
        drift.setLength( drift_length );
        try { tmp.addElement( &drift ); } catch ( Exception& e ) { e.dump(); }
      }
      try { tmp.addElement( elem ); } catch ( Exception& e ) { e.dump(); }
      pos = elem->s()+elem->length();
    }

    // replace the current beamline content with the sequenced one
    clear();
    setElements( tmp.elements() );

    // make sure that the sequence cannot be computed again
    drifts_added_ = true;
  }

  void
  Beamline::setElements( const ElementsMap& elem_map )
  {
    for ( ElementsMap::const_iterator it=elem_map.begin(); it!=elem_map.end(); it++ ) {
      addElement( ( *( it ) )->clone() );
    }
  }
}
