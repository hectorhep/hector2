#ifndef Hector_Elements_ElementDict_h
#define Hector_Elements_ElementDict_h

#include "ApertureBase.h"
#include "ElementBase.h"
#include <string>
#include <map>

namespace Hector
{
  /// Mapping tool between MAD-X nomenclature of beamline objects and Hector objects
  class ElementDictionary
  {
    public:
      /// Static getter for the dictionary
      static ElementDictionary& get();

      /// Get a Hector aperture type from a MAD-X aperture string
      Aperture::Type apertureType( const std::string& str ) const {
        std::map<std::string,Aperture::Type>::const_iterator it = apertype_map_.find( str );
        if ( it!=apertype_map_.end() ) return it->second;
        return Aperture::anInvalidType;
      }
      /// Get a Hector aperture type from a MAD-X aperture string
      Aperture::Type apertureType( const char* str ) const { return apertureType( std::string( str ) ); }
      /// Get a MAD-X apertur string from a Hector aperture type
      std::string apertureTypeStr( const Aperture::Type& type ) const {
        std::map<std::string,Aperture::Type>::const_iterator it;
        for ( it=apertype_map_.begin(); it!=apertype_map_.end(); it++ ) {
          if ( it->second==type ) return it->first;
        }
        return "";
      }

      /// Get a Hector element type from a MAD-X element string
      Element::Type elementType( const std::string& str ) const {
        std::map<std::string,Element::Type>::const_iterator it = elemtype_map_.find( str );
        if ( it!=elemtype_map_.end() ) return it->second;
        return Element::anInvalidElement;
      }
      /// Get a Hector element type from a MAD-X element string
      Element::Type elementType( const char* str ) const { return elementType( std::string( str ) ); }
      /// Get a MAD-X element string from a Hector element type
      std::string elementTypeStr( const Element::Type& type ) const {
        std::map<std::string,Element::Type>::const_iterator it;
        for ( it=elemtype_map_.begin(); it!=elemtype_map_.end(); it++ ) {
          if ( it->second==type ) return it->first;
        }
        return "";
      }

    private:
      ElementDictionary();
      ElementDictionary( const ElementDictionary& );
      void operator=( const ElementDictionary& );

      std::map<std::string,Aperture::Type> apertype_map_;
      std::map<std::string,Element::Type> elemtype_map_;
  };
}

#endif
