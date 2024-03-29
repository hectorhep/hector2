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

#ifndef Hector_Exception_h
#define Hector_Exception_h

#include <iosfwd>
#include <sstream>
#include <string>

#include "Hector/ExceptionType.h"

#define H_DEBUG hector::Exception(__PRETTY_FUNCTION__, hector::ExceptionType::debug)
#define H_INFO hector::Exception(__PRETTY_FUNCTION__, hector::ExceptionType::info)
#define H_WARNING hector::Exception(__PRETTY_FUNCTION__, hector::ExceptionType::warning)
#define H_ERROR hector::Exception(__PRETTY_FUNCTION__, hector::ExceptionType::fatal)

namespace hector {
  /// A simple exception handler
  class Exception : public std::exception {
  public:
    /// Construct an exception from a std::string description
    /// \param[in] from Method/function that raised the exception
    /// \param[in] desc Error message
    /// \param[in] type Type of exception encountered
    /// \param[in] id Error code associated to the exception
    explicit Exception(const std::string& from, ExceptionType type = ExceptionType::undefined, int id = 0);
    /// Copy constructor
    Exception(const Exception& rhs);

    /// Destruct the exception (and terminate the program execution if fatal)
    ~Exception() noexcept override;

    //----- Overloaded stream operators

    /// Generic templated message feeder operator
    template <typename T>
    inline friend const Exception& operator<<(const Exception& exc, T var) {
      Exception& nc_except = const_cast<Exception&>(exc);
      nc_except.message_ << var;
      return exc;
    }
    /// Pipe modifier operator
    inline friend const Exception& operator<<(const Exception& exc, std::ios_base& (*f)(std::ios_base&)) {
      Exception& nc_except = const_cast<Exception&>(exc);
      f(nc_except.message_);
      return exc;
    }

    /// Lambda function handler
    template <typename T>
    inline Exception& log(T&& lam) {
      lam(*this);
      return *this;
    }

    /// Method/function that raised the exception
    inline const std::string from() const { return from_; }
    /// Error code associated to the exception
    inline int errorNumber() const { return error_num_; }
    /// Type of exception encountered (info, warning, fatal error)
    inline ExceptionType type() const { return type_; }
    /// Prettified (colourised) string of the exception type
    const std::string typeString() const;

    /// Print all information about this exception
    void dump(std::ostream& os) const;
    /// Get a one-line description of the error
    const std::string oneLine() const;

  protected:
    std::ostringstream message_;

  private:
    std::string from_;
    ExceptionType type_;
    int error_num_;
  };
}  // namespace hector

#endif
