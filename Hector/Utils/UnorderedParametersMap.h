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

#ifndef Hector_Core_UnorderedParametersMap_h
#define Hector_Core_UnorderedParametersMap_h

#include <stdexcept>
#include <string>
#include <vector>

namespace hector {
  namespace pmap {
    /// Unordered set of parameters indexed by key
    template <class T>
    class Unordered : private std::vector<std::pair<std::string, T> > {
    public:
      Unordered() {}
      ~Unordered() {}

      /// Number of keys stored in the map
      size_t size() const { return map::size(); }

      /// Does the map have this key?
      bool hasKey(const std::string& key) const {
        for (const auto& val : *this) {
          if (val.first == std::string(key))
            return true;
        }
        return false;
      }
      /// Add a new key-value combination
      void add(const std::string& key, const T& value) { map::push_back(std::pair<std::string, T>(key, value)); }

      /// Retrieve the key associated to a position in the map
      const std::string key(const size_t i) const {
        if (i >= map::size())
          throw std::runtime_error("Invalid index: " + std::to_string(i) + " for an unordered map of size " +
                                   std::to_string(map::size()) + ".");
        return map::at(i).first;
      }
      /// Retrieve the value associated to a position in the map
      const T& value(const size_t i) const {
        if (i >= map::size())
          throw std::runtime_error("Invalid index: " + std::to_string(i) + " for an unordered map of size " +
                                   std::to_string(map::size()) + ".");
        return map::at(i).second;
      }
      /// Retrieve the position in the map for the given key
      size_t id(const std::string& k) const {
        for (size_t i = 0; i < map::size(); ++i)
          if (key(i) == k)
            return i;
        throw std::runtime_error("Key \"" + k + "\" was not found in the map.");
      }

      /// Retrieve the value associated to a key
      const T get(const std::string& key) const {
        for (const auto& val : *this)
          if (val.first == key)
            return val.second;
        return static_cast<T>(0);
      }

    public:
      /// Constant-type iterator to browse the map
      typedef typename std::vector<std::pair<std::string, T> >::const_iterator const_iterator;

    private:
      typedef std::vector<std::pair<std::string, T> > map;
    };
  }  // namespace pmap
}  // namespace hector

#endif
