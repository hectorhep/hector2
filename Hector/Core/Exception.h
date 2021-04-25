#ifndef Hector_Core_Exception_h
#define Hector_Core_Exception_h

#include "Hector/Utils/Utils.h"
#include "Hector/Core/Parameters.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>  // exit()

#define PrintDebug Hector::Exception(__PRETTY_FUNCTION__, Hector::Debug)
#define PrintInfo Hector::Exception(__PRETTY_FUNCTION__, Hector::Info)
#define PrintWarning Hector::Exception(__PRETTY_FUNCTION__, Hector::JustWarning)

namespace Hector {
  /// A simple exception handler
  class Exception : public std::exception {
  public:
    /// Construct an exception from a std::string description
    /// \param[in] from Method/function that raised the exception
    /// \param[in] desc Error message
    /// \param[in] type Type of exception encountered
    /// \param[in] id Error code associated to the exception
    explicit inline Exception(const char* from, ExceptionType type = Undefined, int id = 0)
        : from_(from), type_(type), error_num_(id) {}
    /// Copy constructor
    inline Exception(const Exception& rhs)
        : message_(rhs.message_.str()), from_(rhs.from_), type_(rhs.type_), error_num_(rhs.error_num_) {}

    /// Destruct the exception (and terminate the program execution if fatal)
    inline ~Exception() noexcept override {
      dump();
      if (type_ == Fatal)
        exit(error_num_);  // we stop the execution of this process on fatal exception
    }

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

    /// Method/function that raised the exception
    inline const std::string from() const { return from_; }
    /// Error code associated to the exception
    inline int errorNumber() const { return error_num_; }
    /// Type of exception encountered (info, warning, fatal error)
    inline ExceptionType type() const { return type_; }
    /// Prettified (colourised) string of the exception type
    inline const std::string typeString() const {
      switch (type_) {
        case JustWarning:
          return "\033[32;1mJustWarning\033[0m";
        case Info:
          return "\033[33;1mInfo\033[0m";
        case Fatal:
          return "\033[31;1mFatal\033[0m";
        case Undefined:
        default:
          return "\33[7;1mUndefined\033[0m";
      }
    }

    /// Print all information about this exception
    inline void dump(std::ostream& os = std::cerr) const {
      if (type_ < Parameters::get()->loggingThreshold())
        return;
      if (type_ == Info) {
        os << "======================= \033[33;1mInformation\033[0m =======================" << std::endl
           << " From:        " << from_ << std::endl;
      } else {
        os << "=================== Exception detected! ===================" << std::endl
           << " Class:       " << typeString() << std::endl
           << " Raised by:   " << from_ << std::endl;
      }
      os << " Description: " << std::endl << "\t" << message_.str() << std::endl;
      if (error_num_ != 0)
        os << "-----------------------------------------------------------" << std::endl
           << " Error #" << errorNumber() << std::endl;
      os << "===========================================================" << std::endl;
    }
    /// Get a one-line description of the error
    inline const std::string oneLine() const {
      return Form("[%s] ::: %s ::: %s", type_, from_.c_str(), message_.str().c_str());
    }

  protected:
    std::ostringstream message_;

  private:
    std::string from_;
    ExceptionType type_;
    int error_num_;
  };
}  // namespace Hector

#endif
