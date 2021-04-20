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
    if (type_ == Fatal)
      exit(error_num_);  // we stop the execution of this process on fatal exception
  }

  const std::string Exception::typeString() const {
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

  void Exception::dump(std::ostream& os) const {
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

  const std::string Exception::oneLine() const {
    return format("[%s] ::: %s ::: %s", type_, from_.c_str(), message_.str().c_str());
  }
}  // namespace hector
