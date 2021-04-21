#include "Hector/Utils/String.h"

#include <cstdarg>  // For va_start, etc.
#include <algorithm>

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

  std::string now() {
    static char buffer[25];
    time_t raw_time;
    time(&raw_time);
    struct tm* time_info = localtime(&raw_time);
    strftime(buffer, 25, "%Y-%d-%m %H:%M:%S", time_info);
    return std::string(buffer);
  }
}  // namespace hector
