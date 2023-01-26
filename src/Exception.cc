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

#include <cstdlib>  // exit()
#include <iomanip>
#include <iostream>

#include "Hector/Exception.h"
#include "Hector/Parameters.h"
#include "Hector/Utils/String.h"

namespace hector {
  Exception::Exception(const std::string& from, ExceptionType type, int id)
      : from_(from), type_(type), error_num_(id) {}

  Exception::Exception(const Exception& rhs)
      : message_(rhs.message_.str()), from_(rhs.from_), type_(rhs.type_), error_num_(rhs.error_num_) {}

  Exception::~Exception() noexcept {
    dump(std::cerr);
    if (type_ == ExceptionType::fatal)
      exit(error_num_);  // we stop the execution of this process on fatal exception
  }

  const std::string Exception::typeString() const {
    switch (type_) {
      case ExceptionType::debug:
        return colourise("Debug:", Colour::yellow, Modifier::reverse);
      case ExceptionType::warning:
        return colourise("Warning:", Colour::blue, Modifier::bold);
      case ExceptionType::info:
        return colourise("Info:", Colour::green, Modifier::bold);
      case ExceptionType::fatal:
        return colourise("Error", Colour::red, Modifier::bold);
      case ExceptionType::undefined:
      default:
        return colourise("Undefined exception", Colour::reset, Modifier::reverse);
    }
  }

  void Exception::dump(std::ostream& os) const {
    if (type_ < Parameters::get()->loggingThreshold())
      return;
    os << typeString();
    switch (type_) {
      case ExceptionType::info:
        os << "\t" << message_.str() << "\n";
        break;
      case ExceptionType::debug:
      case ExceptionType::warning:
        os << " " << colourise(from_, Colour::reset, Modifier::underline) << "\n\t" << message_.str() << "\n";
        break;
      case ExceptionType::undefined:
      case ExceptionType::fatal: {
        const std::string sep(80, '-');
        os << sep << "\n"
           << " occurred at " << now() << "\n";
        if (!from_.empty())
          os << "  raised by: " << colourise(from_, Colour::reset, Modifier::underline) << "\n";
        if (errorNumber() != 0)
          os << "  error #" << error_num_ << "\n";
        os << "\n" << message_.str() << "\n" << sep << "\n";
      } break;
    }
    os << std::flush;
  }

  const std::string Exception::oneLine() const {
    return format("[%s] ::: %s ::: %s", type_, from_.c_str(), message_.str().c_str());
  }
}  // namespace hector
