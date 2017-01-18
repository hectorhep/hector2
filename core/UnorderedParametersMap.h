#ifndef Core_UnorderedParametersMap_h
#define Core_UnorderedParametersMap_h

#include <vector>
#include <string>

using std::cout;

namespace Hector
{
  namespace ParametersMap
  {
    /// Unordered set of parameters indexed by key
    template<class T>
    class Unordered : private std::vector< std::pair<std::string,T> >
    {
      public:
        Unordered() {}
        ~Unordered() {}

        /// Number of keys stored in the map
        size_t size() const { return map::size(); }

        /// Does the map have this key?
        bool hasKey( const char* key ) const {
          for ( const_iterator it=map::begin(); it!=map::end(); it++ ) {
            if ( it->first==std::string( key ) ) return true;
          }
          return false;
        }
        /// Add a new key-value combination
        void add( const std::string& key, const T& value ) {
          map::push_back( std::pair<std::string,T>( key, value ) );
        }
        /// Add a new key-value combination
        void add( const char* key, const T& value ) { add( std::string( key ), value ); }

        /// Retrieve the key associated to a position in the map
        const std::string key( const size_t i ) const {
          if ( i>=map::size() ) {
            throw Exception( __PRETTY_FUNCTION__, Form( "Invalid index: %d for an unordered map of size %d", i, map::size() ), JustWarning );
          }
          return map::at( i ).first;
        }
        /// Retrieve the value associated to a position in the map
        const T value( const size_t i ) const {
          if ( i>=map::size() ) {
            throw Exception( __PRETTY_FUNCTION__, Form( "Invalid index: %d for an unordered map of size %d", i, map::size() ), JustWarning );
          }
          return map::at( i ).second;
        }
        /// Retrieve the position in the map for the given key
        const size_t id( const char* k ) const {
          for ( size_t i=0; i<map::size(); i++ ) {
            if ( key( i )==k ) return i;
          }
          throw Exception( __PRETTY_FUNCTION__, Form( "Key \"%s\" was not found in the map", k ), JustWarning );
          //return 9999;
        }

        /// Retrieve the value associated to a key
        const T get( const char* key ) const {
          for ( const_iterator it=map::begin(); it!=map::end(); it++ ) {
            if ( it->first==std::string( key ) ) return it->second;
          }
          return static_cast<T>( 0 );
        }

        /// Print the whole list of key-values stored in the map
        void dump( std::ostream& os=std::cout ) const {
          for ( const_iterator p=map::begin(); p!=map::end(); p++ ) {
            os << " [" << p->first << "] " << p->second << std::endl;
          }
        }

      public:
        /// Constant-type iterator to browse the map
        typedef typename std::vector< std::pair<std::string,T> >::const_iterator const_iterator;

      private:
        typedef std::vector< std::pair<std::string,T> > map;
    };
  }
}

#endif
