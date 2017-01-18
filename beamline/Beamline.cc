#include "Beamline.h"

namespace Hector
{
  Beamline::Beamline() :
    max_length_( 0. ), has_next_element_( false )
  {
  }

  Beamline::Beamline( float length, const CLHEP::Hep3Vector& ip ) :
    max_length_( length ), ip_( ip ), has_next_element_( false )
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
      if ( *elem ) delete *elem;
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
    if ( delete_after ) delete elem;
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

  CLHEP::HepMatrix
  Beamline::matrix( float eloss, float mp, int qp )
  {
    CLHEP::HepMatrix out = CLHEP::HepDiagMatrix( 6, 1 );

    for ( size_t i=0; i<elements_.size(); i++ ) {
      Element::ElementBase* elem = elements_.at( i );
      out = out*elem->matrix( eloss, mp, qp );
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
}
