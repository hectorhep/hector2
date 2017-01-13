#ifndef Core_ParametersMap_h
#define Core_ParametersMap_h

#include <map>
#include <string>

using std::cout;

namespace Hector
{
  /// Ordered set of parameters indexed by key
  template<class T>
  class ParametersMap : public std::map<std::string,T>
  {
    public:
      ParametersMap() {}
      ~ParametersMap() {}

      /// Does the map have this key?
      bool hasKey( const char* key ) const { return map::find( std::string( key ) )!=map::end(); }
      /// Add a new key-value combination
      void add( const std::string& key, const T& value ) {
        map::insert( std::pair<std::string,T>( key, value ) );
      }
      /// Add a new key-value combination
      void add( const char* key, const T& value ) { add( std::string( key ), value ); }
      /// Retrieve the value associated to a key
      const T get( const char* key ) const {
        const_iterator p = map::find( std::string( key ) );
        return ( p!=map::end() ) ? p->second : static_cast<T>( 0 );
      }

      /// Print the whole list of key-values stored in the map
      void dump( std::ostream& os=std::cout ) const {
        for ( const_iterator p=map::begin(); p!=map::end(); p++ ) {
          os << " [" << p->first << "] " << p->second << std::endl;
        }
      }

    public:
      /// Constant-type iterator to browse the map
      typedef typename std::map<std::string,T>::const_iterator const_iterator;

    private:
      typedef std::map<std::string,T> map;
  };
}

#endif
