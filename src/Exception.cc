#include "Hector/Exception.h"

#include "Hector/Utils/String.h"
#include "Hector/Parameters.h"

#include <iomanip>
#include <iostream>
#include <cstdlib>  // exit()

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
      case ExceptionType::warning:
        return "\033[32;1mJustWarning\033[0m";
      case ExceptionType::info:
        return "\033[33;1mInfo\033[0m";
      case ExceptionType::fatal:
        return "\033[31;1mFatal\033[0m";
      case ExceptionType::undefined:
      default:
        return "\33[7;1mUndefined\033[0m";
    }
  }

  void Exception::dump(std::ostream& os) const {
    if (type_ < Parameters::get()->loggingThreshold())
      return;
    switch (type_) {
      case ExceptionType::info:
        os << colourise("Info:", Colour::green, Modifier::bold) << "\t" << message_.str() << "\n";
        break;
      case ExceptionType::debug:
        os << colourise("Debug:", Colour::yellow, Modifier::reverse) << " "
           << colourise(from_, Colour::reset, Modifier::underline) << "\n\t" << message_.str() << "\n";
        break;
      case ExceptionType::warning:
        os << colourise("Warning:", Colour::blue, Modifier::bold) << " "
           << colourise(from_, Colour::reset, Modifier::underline) << "\n\t" << message_.str() << "\n";
        break;
      case ExceptionType::undefined:
      case ExceptionType::fatal: {
        const std::string sep(80, '-');
        os << sep << "\n";
        if (type_ == ExceptionType::fatal)
          os << colourise("Error", Colour::red, Modifier::bold);
        else if (type_ == ExceptionType::undefined)
          os << colourise("Undefined exception", Colour::reset, Modifier::reverse);
        os << " occurred at " << now() << "\n";
        if (!from_.empty())
          os << "  raised by: " << colourise(from_, Colour::reset, Modifier::underline) << "\n";
        if (errorNumber() != 0)
          os << "  error #" << error_num_ << "\n";
        os << "\n" << message_.str() << "\n";
        os << sep << "\n";
      } break;
    }
    os << std::flush;
  }

  const std::string Exception::oneLine() const {
    return format("[%s] ::: %s ::: %s", type_, from_.c_str(), message_.str().c_str());
  }
}  // namespace hector
