#ifndef Hector_Core_ParametersMap_h
#define Hector_Core_ParametersMap_h

#include <map>
#include <string>
#include <iosfwd>
#include <stdexcept>

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
      bool hasKey(const std::string& key) const { return map::count(key) != 0; }
      /// Add a new key-value combination
      void add(const std::string& key, const T& value) { map::insert(std::pair<std::string, T>(key, value)); }
      /// Retrieve the value associated to a key
      const T& get(const std::string& key) const {
        const auto& val = map::find(key);
        if (val == map::end())
          throw std::out_of_range("Failed to retrieve key!");
        return val->second;
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
