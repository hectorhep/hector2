/*
 *  Hector: a beamline propagation tool
 *  Copyright (C) 2016-2023  Laurent Forthomme
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef Hector_Core_ParametersMap_h
#define Hector_Core_ParametersMap_h

#include <iosfwd>
#include <map>
#include <stdexcept>
#include <string>

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
