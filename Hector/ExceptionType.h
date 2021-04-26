#ifndef Hector_ExceptionType_h
#define Hector_ExceptionType_h

namespace hector {
  /// Enumeration of exception severities
  enum struct ExceptionType {
    undefined = -1,  ///< Invalid exception type
    debug,           ///< Debugging information for developers
    info,            ///< Simple log to interact with the end user
    warning,         ///< Simple warning (proceeds with the execution)
    fatal            ///< Fatal error (exits the program)
  };
}  // namespace hector

#endif
