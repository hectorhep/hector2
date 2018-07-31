#include "Hector/Beamline/Beamline.h"
#include "Hector/Core/Exception.h"
#include "Hector/Elements/Drift.h"
#include "Hector/Propagator/Particle.h"

#include <sstream>
#include <algorithm>
#include <regex>

namespace Hector
{
  Beamline::Beamline() :
    max_length_( 0. )
  {}

  Beamline::Beamline( const Beamline& rhs, bool copy_elements ) :
    max_length_( rhs.max_length_ ), ip_( rhs.ip_ ),
    markers_( rhs.markers_ )
  {
    clear();
    if ( copy_elements )
      setElements( rhs );
  }

  Beamline::Beamline( double length, const ThreeVector& ip ) :
    max_length_( length+5. ), // artificially increase the size to include next elements
    ip_( ip )
  {}

  Beamline::~Beamline()
  {
    clear();
    markers_.clear();
  }

  void
  Beamline::clear()
  {
    elements_.clear();
  }

  void
  Beamline::addMarker( const Element::Marker& marker )
  {
    markers_.insert( std::pair<double,Element::Marker>( marker.s(), marker ) );
  }

  void
  Beamline::add( const std::shared_ptr<Element::ElementBase> elem )
  {
    const double new_size = elem->s()+elem->length();
    if ( new_size > max_length_ && max_length_ < 0. )
      throw Exception( __PRETTY_FUNCTION__, Form( "Element %s is too far away for this beamline!\n"
                                                  "\tBeamline length: %.3f m, this element: %.3f m",
                                                  elem->name().c_str(), max_length_, new_size ), Debug );

    bool already_added = false;

    // check the overlaps before adding
    for ( auto& prev_elem : elements_ ) {
      // first check if the element is already present in the beamline
      if ( prev_elem->name() == elem->name() ) {
        already_added = true;
        break;
      }

      if ( prev_elem->s() > elem->s() ) break;
      if ( prev_elem->s()+prev_elem->length() <= elem->s() ) continue;
      if ( prev_elem->length() == 0 ) continue;
      if ( prev_elem->s() == elem->s() && elem->length() == 0 ) continue;

      if ( !Parameters::get()->correctBeamlineOverlaps() )
        throw Exception( __PRETTY_FUNCTION__, Form( "Elements overlap with \"%s\" "
                                                    "detected while adding \"%s\"!",
                                                    prev_elem->name().c_str(), elem->name().c_str() ), FatalError );

      // from that point on, an overlap is detected
      // reduce or separate that element in two sub-parts

      PrintDebug( Form( "%s (%s) is inside %s (%s)\n\t"
                        "Hector will fix the overlap by splitting the earlier.",
                        elem->name().c_str(), elem->typeName().c_str(),
                        prev_elem->name().c_str(), prev_elem->typeName().c_str() ) );
      const double prev_length = prev_elem->length();

      std::shared_ptr<Element::ElementBase> next_elem = nullptr;
      // check if one needs to add an extra piece to the previous element
      if ( elem->s()+elem->length() < prev_elem->s()+prev_elem->length() ) {
        const std::string prev_name = prev_elem->name();
        prev_elem->setName( Form( "%s/1", prev_name.c_str() ) );
        next_elem = prev_elem->clone();
        next_elem->setName( Form( "%s/2", prev_name.c_str() ) );
        next_elem->setS( elem->s()+elem->length() );
        next_elem->setLength( prev_length-elem->length() );
        next_elem->setBeta( elem->beta() );
        next_elem->setDispersion( elem->dispersion() );
        next_elem->setRelativePosition( elem->relativePosition() );
        next_elem->setParentElement( prev_elem );
      }

      prev_elem->setLength( elem->s()-prev_elem->s() );

      elements_.push_back( elem );
      already_added = true;

      if ( next_elem != nullptr )
        elements_.push_back( next_elem );

      break;
    }

    if ( !already_added )
      elements_.push_back( elem );

    // sort all beamline elements according to their s-position
    std::sort( elements_.begin(), elements_.end(), Element::ElementsSorter() );
  }

  const std::shared_ptr<Element::ElementBase>
  Beamline::get( const char* name ) const
  {
    for ( size_t i = 0; i < elements_.size(); ++i ) {
      const auto elem = elements_.at( i );
      if ( elem->name().find( name ) != std::string::npos ) return elem;
    }
    return 0;
  }

  std::shared_ptr<Element::ElementBase>&
  Beamline::get( const char* name )
  {
    for ( auto& elem : elements_ ) {
      if ( elem->name().find( name ) != std::string::npos ) return elem;
    }
    return *elements_.end();
  }

  const std::shared_ptr<Element::ElementBase>
  Beamline::get( double s ) const
  {
    for ( size_t i = 0; i < elements_.size(); ++i ) {
      const auto elem = elements_.at( i );
      if ( elem->s() > s ) continue;
      if ( elem->s() <= s && elem->s()+elem->length() >= s ) return elem;
    }
    return 0;
  }

  std::shared_ptr<Element::ElementBase>&
  Beamline::get( double s )
  {
    for ( auto& elem : elements_ ) {
      if ( elem->s() > s ) continue;
      if ( elem->s() <= s && elem->s()+elem->length() >= s ) return elem;
    }
    return *elements_.end();
  }

  std::vector<std::shared_ptr<Element::ElementBase> >
  Beamline::find( const char* regex )
  {
    try {
      std::regex rgx_search( regex );
      std::cmatch m;
      std::vector<std::shared_ptr<Element::ElementBase> > out;
      for ( auto& elem : elements_ )
        if ( std::regex_search( elem->name().c_str(), m, rgx_search ) ) out.emplace_back( elem );
      return out;
    } catch ( const std::regex_error& e ) {
      throw Exception( __PRETTY_FUNCTION__, Form( "Invalid regular expression required:\n\t%s\n\tError code: %d", regex, e.code() ), FatalError );
    }
  }

  Matrix
  Beamline::matrix( double eloss, double mp, int qp ) const
  {
    Matrix out = DiagonalMatrix( 6, 1 );

    for ( const auto& elem : elements_ )
      out = out * elem->matrix( eloss, mp, qp );

    return out;
  }

  double
  Beamline::length() const
  {
    if ( elements_.empty() ) return 0.;
    const auto elem = *elements_.rbegin();
    return ( elem->s() + elem->length() );
  }

  void
  Beamline::dump( std::ostream& os, bool show_drifts ) const
  {
    os << "=============== Beamline dump ===============\n"
       << " interaction point: " << interactionPoint() << "\n"
       << " length: " << length() << " m\n"
       << " elements list: ";
    for ( const auto& elemPtr : elements_ ) {
      if ( !show_drifts && elemPtr->type() == Element::aDrift ) continue;
      os << "\n  * " << *elemPtr;
    }
    os << "\n";
  }

  void
  Beamline::offsetElementsAfter( double s, const TwoVector& offset )
  {
    for ( auto& elemPtr : elements_ ) {
      if ( elemPtr->s() < s ) continue;
      elemPtr->offset( offset );
    }
  }

  void
  Beamline::tiltElementsAfter( double s, const TwoVector& tilt )
  {
    for ( auto& elemPtr : elements_ ) {
      if ( elemPtr->s() < s ) continue;
      elemPtr->tilt( tilt );
    }
  }

  std::unique_ptr<Beamline>
  Beamline::sequencedBeamline( const Beamline* beamline )
  {
    // add the drifts between optical elements
    double pos = 0.;
    // brand new beamline to populate
    std::unique_ptr<Beamline> tmp( new Beamline( *beamline, false ) );

    // convert all empty spaces into drifts
    for ( const auto& elemPtr : *beamline ) {
      // skip the markers
      if ( elemPtr->type() == Element::aMarker && elemPtr->s() != beamline->interactionPoint().z() )
        continue;
      // add a drift whenever there is a gap in s
      const double drift_length = elemPtr->s()-pos;
      try {
        if ( drift_length > 0. )
          tmp->add( std::make_shared<Element::Drift>( Form( "drift:%.4E", pos ).c_str(), pos, drift_length ) );
        tmp->add( elemPtr );
      } catch ( Exception& e ) { e.dump(); }
      pos = elemPtr->s()+elemPtr->length();
    }
    // add the last drift
    const double drift_length = tmp->length()-pos;
    if ( drift_length > 0 ) {
      try {
        tmp->add( std::make_shared<Element::Drift>( Form( "drift:%.4E", pos ).c_str(), pos, drift_length ) );
      } catch ( Exception& e ) { e.dump(); }
    }

    return tmp;
  }

  void
  Beamline::setElements( const Beamline& moth_bl )
  {
    for ( const auto& elem : moth_bl ) add( elem );
  }
}
