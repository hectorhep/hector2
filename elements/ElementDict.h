#ifndef Elements_ElementDict_h
#define Elements_ElementDict_h

#include "ApertureBase.h"
#include "ElementBase.h"
#include <string>
#include <map>

namespace Element
{
  class Dictionary
  {
    public:
      ~Dictionary() { built_ = false; }
      static Dictionary* get();

      ApertureBase::Type apertureType( const std::string& str ) const {
        std::map<std::string,ApertureBase::Type>::const_iterator it = apertype_map_.find( str );
        if ( it==apertype_map_.end() ) return ApertureBase::None;
        return it->second;
      }
      ApertureBase::Type apertureType( const char* str ) const { return apertureType( std::string( str ) ); }
      std::string apertureTypeStr( const ApertureBase::Type& type ) const {
        for ( std::map<std::string,ApertureBase::Type>::const_iterator it=apertype_map_.begin(); it!=apertype_map_.end(); it++ ) {
          if ( it->second==type ) return it->first;
        }
        return "";
      }

      ElementBase::Type elementType( const std::string& str ) const {
        std::map<std::string,ElementBase::Type>::const_iterator it = elemtype_map_.find( str );
        if ( it==elemtype_map_.end() ) return ElementBase::Invalid;
        return it->second;
      }
      ElementBase::Type elementType( const char* str ) const { return elementType( std::string( str ) ); }
      std::string elementTypeStr( const ElementBase::Type& type ) const {
        for ( std::map<std::string,ElementBase::Type>::const_iterator it=elemtype_map_.begin(); it!=elemtype_map_.end(); it++ ) {
          if ( it->second==type ) return it->first;
        }
        return "";
      }

    private:
      Dictionary();
      static Dictionary* dictionary_;
      static bool built_;

      std::map<std::string,ApertureBase::Type> apertype_map_;
      std::map<std::string,ElementBase::Type> elemtype_map_;
  };
}

#endif
