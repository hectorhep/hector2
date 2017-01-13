#ifndef Exception_h
#define Exception_h

#include "Utils.h"

#include <iomanip>
#include <iostream>
#include <string>
#include <cstdlib> // exit()

#define PrintInfo(m) Exception(__PRETTY_FUNCTION__, m, Info).dump();

using std::cerr;

namespace Hector
{
  /**
   * \brief Enumeration of exception severities
   * \author Laurent Forthomme <laurent.forthomme@cern.ch>
   * \date 27 Mar 2015
   */
  typedef enum { Undefined=-1, Info, JustWarning, Fatal } ExceptionType;

  /**
   * \brief A simple exception handler
   * \author Laurent Forthomme <laurent.forthomme@cern.ch>
   * \date 24 Mar 2015
   */
  class Exception
  {
    public:
      inline Exception( const char* from, std::string desc, ExceptionType type=Undefined, const int id=0 ) :
        from_( from ), description_( desc ), type_( type ), error_num_( id ) {}
      
      inline Exception( const char* from, const char* desc, ExceptionType type=Undefined, const int id=0 ) :
        from_( from ), description_( desc ), type_( type ), error_num_( id ) {}

      inline ~Exception() {
        if ( type()==Fatal ) exit(0);
        // we stop this process' execution on fatal exception
      }
      
      inline const std::string from() const { return from_; }
      inline const int errorNumber() const { return error_num_; }
      inline const std::string description() const { return description_; }
      inline const ExceptionType type() const { return type_; }
      inline const std::string typeString() const {
        switch ( type() ) {
          case JustWarning:        return "\033[34;1mJustWarning\033[0m";
          case Info:               return "\033[33;1mInfo\033[0m";
          case Fatal:              return "\033[31;1mFatal\033[0m";
          case Undefined: default: return "\33[7;1mUndefined\033[0m";
        }
      }
      
      inline void dump( std::ostream& os=std::cerr ) const {
        if ( type()==Info ) {
          os << "======================= \033[33;1mInformation\033[0m =======================" << std::endl
             << " From:        " << from() << std::endl;
        }
        else {
          os << "=================== Exception detected! ===================" << std::endl
             << " Class:       " << typeString() << std::endl
             << " Raised by:   " << from() << std::endl;
        }
        os << " Description: " << std::endl
           << "\t" << description() << std::endl;
        if ( errorNumber()!=0 )
          os << "-----------------------------------------------------------" << std::endl
             << " Error #" << errorNumber() << std::endl;
        os << "===========================================================" << std::endl;
      }
      inline const std::string oneLine() const {
        std::ostringstream os;
        os << "[" << type() << "] === " << from() << " === "
           << description();
        return os.str();
      }
      
    private:
      std::string from_;
      std::string description_;
      ExceptionType type_;
      int error_num_;
  };
}

#endif

