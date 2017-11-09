#ifndef Hector_Core_Exception_h
#define Hector_Core_Exception_h

#include "Utils.h"
#include "Parameters.h"

#include <iomanip>
#include <iostream>
#include <string>
#include <cstdlib> // exit()

#define PrintInfo(m) Hector::Exception( __PRETTY_FUNCTION__, m, Hector::Info ).dump();
#define PrintWarning(m) Hector::Exception( __PRETTY_FUNCTION__, m, Hector::JustWarning ).dump();

using std::cerr;

namespace Hector
{
  /// A simple exception handler
  class Exception : public std::runtime_error
  {
    public:
      /// Construct an exception from a std::string description
      /// \param[in] from Method/function that raised the exception
      /// \param[in] desc Error message
      /// \param[in] type Type of exception encountered
      /// \param[in] id Error code associated to the exception
      inline Exception( const char* from, const std::string& desc, ExceptionType type = Undefined, const int id = 0 ) :
        std::runtime_error( desc ), from_( from ), type_( type ), error_num_( id ) {}
      /// Construct an exception from a char* description
      inline Exception( const char* from, const char* desc, ExceptionType type = Undefined, const int id = 0 ) :
        std::runtime_error( desc ), from_( from ), type_( type ), error_num_( id ) {}

      /// Destruct the exception (and terminate the program execution if fatal)
      inline ~Exception() {
        if ( type() == Fatal ) exit( 0 ); // we stop the execution of this process on fatal exception
      }

      /// Method/function that raised the exception
      inline const std::string from() const { return from_; }
      /// Error code associated to the exception
      inline const int errorNumber() const { return error_num_; }
      /// Type of exception encountered (info, warning, fatal error)
      inline const ExceptionType type() const { return type_; }
      /// Prettified (colourised) string of the exception type
      inline const std::string typeString() const {
        switch ( type() ) {
          case JustWarning:        return "\033[32;1mJustWarning\033[0m";
          case Info:               return "\033[33;1mInfo\033[0m";
          case Fatal:              return "\033[31;1mFatal\033[0m";
          case Undefined: default: return "\33[7;1mUndefined\033[0m";
        }
      }

      /// Print all information about this exception
      inline void dump( std::ostream& os = std::cerr ) const {
        if ( type()<Parameters::get()->loggingThreshold() ) return;
        if ( type() == Info ) {
          os << "======================= \033[33;1mInformation\033[0m =======================" << std::endl
             << " From:        " << from() << std::endl;
        }
        else {
          os << "=================== Exception detected! ===================" << std::endl
             << " Class:       " << typeString() << std::endl
             << " Raised by:   " << from() << std::endl;
        }
        os << " Description: " << std::endl
           << "\t" << what() << std::endl;
        if ( errorNumber()!=0 )
          os << "-----------------------------------------------------------" << std::endl
             << " Error #" << errorNumber() << std::endl;
        os << "===========================================================" << std::endl;
      }
      /// Get a one-line description of the error
      inline const std::string oneLine() const {
        std::ostringstream os;
        os << "[" << type() << "] === " << from() << " === "
           << what();
        return os.str();
      }

    private:
      std::string from_;
      ExceptionType type_;
      int error_num_;
  };
}

#endif

