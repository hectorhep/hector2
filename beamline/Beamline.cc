#include "Beamline.h"

Beamline::Beamline() :
  length_( 0. )
{
}

Beamline::Beamline( float length, const CLHEP::Hep3Vector& ip ) :
  length_( length ), ip_( ip )
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

CLHEP::HepMatrix
Beamline::matrix( float eloss, float mp, int qp )
{
  CLHEP::HepMatrix out = CLHEP::HepDiagMatrix( 6, 1 );

  for ( size_t i=0; i<elements_.size(); i++ ) {
    Element::ElementBase* elem = elements_.at( i );
    elem->computeMatrix( eloss, mp, qp );
    //std::cout << "for element " << elem->name() << "(" << elem->type() << ")" << elem->matrix() << std::endl;
    out = out*elem->matrix();
  }

  return out;
}

void
Beamline::propagate( const Particle& part, float s )
{
  for ( size_t i=0; i<elements_.size(); i++ ) {
    Element::ElementBase* elem = elements_.at( i );
    if ( elem->s()==s ) {
      
    }
  }
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
