#ifndef Hector_Core_ParametersMap_h
#define Hector_Core_ParametersMap_h

#include <map>
#include <string>
#include <iostream>

using std::cout;

namespace hector {
  /// Collection of key-value containers
  namespace pmap {
    /// Ordered set of parameters indexed by key
    template <class T>
    class Ordered : private std::map<std::string, T> {
    public:
      Ordered() {}
      ~Ordered() {}

      /// Number of keys stored in the map
      using std::map<std::string, T>::size;

      /// Return a map-typped object
      std::map<std::string, T> asMap() const { return *this; }

      /// Does the map have this key?
      bool hasKey(const char* key) const { return map::find(std::string(key)) != map::end(); }
      /// Add a new key-value combination
      void add(const std::string& key, const T& value) { map::insert(std::pair<std::string, T>(key, value)); }
      /// Add a new key-value combination
      void add(const char* key, const T& value) { add(std::string(key), value); }
      /// Retrieve the value associated to a key
      const T& get(const char* key) const {
        const auto& val = map::find(std::string(key));
        if (val == map::end())
          throw std::out_of_range("Failed to retrieve key!");
        return val->second;
      }

      /// Print the whole list of key-values stored in the map
      void dump(std::ostream& os = std::cout) const {
        for (const auto& val : *this)
          os << " [" << val.first << "] " << val.second << std::endl;
      }

    public:
      /// Constant-type iterator to browse the map
      typedef typename std::map<std::string, T>::const_iterator const_iterator;

    private:
      typedef std::map<std::string, T> map;
    };
  }  // namespace pmap
}  // namespace hector

#endif
