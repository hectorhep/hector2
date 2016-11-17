#include "Beamline.h"

Beamline::Beamline()
{
}

Beamline::~Beamline()
{
  for ( ElementsMap::iterator it=elements_.begin(); it!=elements_.end(); it++ ) {
    if ( *it ) delete *it;
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

  std::cout << "New element added: " << elem << std::endl;
  //elements_.push_back( static_cast<const Element::Element*>( elem )->clone() );
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
for ( size_t j=0; j<6; j++ ) {
  for ( size_t k=0; k<6; k++ ) {
if ( elem->matrix()( j, k ) > 100. )
    std::cout << "for element " << elem->name() << "(" << elem->type() << ")" << elem->matrix() << std::endl;
}}
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
