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

#include <algorithm>
#include <cstdarg>  // For va_start, etc.
#include <locale>

#include "Hector/Utils/String.h"

namespace hector {
  std::string format(const std::string fmt, ...) {
    int size = (static_cast<int>(fmt.size())) * 2 + 50;
    std::string str;
    va_list ap;
    while (true) {
      str.resize(size);
      va_start(ap, fmt);
      int n = vsnprintf((char*)str.data(), size, fmt.c_str(), ap);
      va_end(ap);
      if (n > -1 && n < size) {
        str.resize(n);
        return str;
      }
      if (n > -1)
        size = n + 1;
      else
        size *= 2;  // retry with a larger size
    }
    return str;
  }

  std::string trim(const std::string& str) {
    if (str == "")
      return str;
    size_t first = str.find_first_not_of(' '), last = str.find_last_not_of(' ');
    std::string tmp = str.substr(first, (last - first + 1));
    first = tmp.find_first_not_of('"');
    last = tmp.find_last_not_of('"');
    return tmp.substr(first, (last - first + 1));
  }

  std::string lowercase(const std::string& str) {
    std::string s(str);
    std::transform(str.begin(), str.end(), s.begin(), ::tolower);
    return s;
  }

  std::string colourise(const std::string& str, Colour col, Modifier mod) {
    if (mod == Modifier::reset)
      return format("\033[%dm%s\033[0m", (int)col, str.c_str());
    if (col == Colour::reset)
      return format("\033[%dm%s\033[0m", (int)mod, str.c_str());
    return format("\033[%d;%dm%s\033[0m", (int)col, (int)mod, str.c_str());
  }

  std::string now(const std::string& fmt) {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    char out_str[50];
    strftime(out_str, 50, fmt.c_str(), &tm);
    return std::string(out_str);
  }
}  // namespace hector
