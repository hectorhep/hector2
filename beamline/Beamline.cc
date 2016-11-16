#include "Beamline.h"

Beamline::Beamline()
{
}

Beamline::~Beamline()
{
  for ( ElementsMap::iterator it=elements_.begin(); it!=elements_.end(); it++ ) {
    delete *it;
  }
}

void
Beamline::addElement( const Element::ElementBase* elem )
{
  const float new_size = elem->s()+elem->length();
  if ( new_size>length_ ) {
    if ( length_<0. ) throw Exception( __PRETTY_FUNCTION__, Form( "Element %s is too far away for this beamline!\n\tBeamline length: %.3f m, this element: %.3f m", elem->name().c_str(), length_, new_size ), Fatal );
    return;
  }

  elements_.push_back( elem->clone() );
  std::sort( elements_.begin(), elements_.end(), Element::sorter() );
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
