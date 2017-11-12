#include "Hector/Core/Utils.h"

#include <stdarg.h>  // For va_start, etc.
#include <algorithm>
#include <locale>

namespace Hector
{
  std::string Form( const std::string fmt, ... )
  {
    int size = ( static_cast<int>( fmt.size() ) )*2 + 50;
    std::string str;
    va_list ap;
    while ( true ) {
      str.resize( size );
      va_start( ap, fmt );
      int n = vsnprintf( (char*)str.data(), size, fmt.c_str(), ap );
      va_end( ap );
      if ( n>-1 and n<size ) {
        str.resize( n );
        return str;
      }
      if ( n>-1 ) size = n + 1;
      else size *= 2; // retry with a larger size
    }
    return str;
  }

  std::string trim( const std::string& str ) {
    if ( str=="" ) return str;
    size_t first = str.find_first_not_of( ' ' ),
           last = str.find_last_not_of( ' ' );
    std::string tmp = str.substr( first, ( last-first+1 ) );
    first = str.find_first_not_of( '"' );
    last = str.find_last_not_of( '"' );
    return str.substr( first, ( last-first+1 ) );
  }

  /// Get the lowercase version of a string
  std::string lowercase( const std::string& str ) {
    std::string s( str );
    std::transform( str.begin(), str.end(), s.begin(), ::tolower );
    return s;
  }
}
