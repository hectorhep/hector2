#ifndef Core_UnorderedParametersMap_h
#define Core_UnorderedParametersMap_h

#include <vector>
#include <string>

using std::cout;

template<class T>
class UnorderedParametersMap : public std::vector< std::pair<std::string,T> >
{
  public:
    UnorderedParametersMap() {;}
    ~UnorderedParametersMap() {;}

    bool hasKey( const char* key ) const {
      for ( const_iterator it=map::begin(); it!=map::end(); it++ ) {
        if ( it->first==std::string( key ) ) return true;
      }
      return false;
    }
    void add( const std::string& key, const T& value ) {
      map::push_back( std::pair<std::string,T>( key, value ) );
    }
    void add( const char* key, const T& value ) { add( std::string( key ), value ); }

    const std::string key( const size_t i ) const {
      if ( i>=map::size() ) {
        throw Exception( __PRETTY_FUNCTION__, Form( "Invalid index: %d for an unordered map of size %d", i, map::size() ), JustWarning );
      }
      return map::at( i ).first;
    }
    const T value( const size_t i ) const {
      if ( i>=map::size() ) {
        throw Exception( __PRETTY_FUNCTION__, Form( "Invalid index: %d for an unordered map of size %d", i, map::size() ), JustWarning );
      }
      return map::at( i ).second;
    }
    const size_t id( const char* k ) const {
      for ( size_t i=0; i<map::size(); i++ ) {
        if ( key( i )==k ) return i;
      }
      throw Exception( __PRETTY_FUNCTION__, Form( "Key \"%s\" was not found in the map", k ), JustWarning );
      //return 9999;
    }

    const T get( const char* key ) const {
      for ( const_iterator it=map::begin(); it!=map::end(); it++ ) {
        if ( it->first==std::string( key ) ) return it->second;
      }
      return static_cast<T>( 0 );
    }

    void dump( std::ostream& os=std::cout ) const {
      for ( const_iterator p=map::begin(); p!=map::end(); p++ ) {
        os << " [" << p->first << "] " << p->second << std::endl;
      }
    }

  public:
    typedef typename std::vector< std::pair<std::string,T> >::const_iterator const_iterator;

  private:
    typedef std::vector< std::pair<std::string,T> > map;
};

#endif
