#ifndef Hector_Core_ExceptionType_h
#define Hector_Core_ExceptionType_h

namespace Hector
{
  /// Enumeration of exception severities
  typedef enum {
    Undefined = -1, ///< Invalid exception type
    Debug, ///< Debugging information for developers
    Info, ///< Simple log to interact with the end user
    JustWarning, ///< Simple warning (proceeds with the execution)
    FatalError ///< Fatal error (exits the program)
  } ExceptionType;
}

#endif
