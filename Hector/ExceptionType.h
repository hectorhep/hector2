#ifndef Hector_ExceptionType_h
#define Hector_ExceptionType_h

namespace hector {
  /// Enumeration of exception severities
  typedef enum {
    Undefined = -1,  ///< Invalid exception type
    Debug,           ///< Debugging information for developers
    Info,            ///< Simple log to interact with the end user
    JustWarning,     ///< Simple warning (proceeds with the execution)
    Fatal            ///< Fatal error (exits the program)
  } ExceptionType;
}  // namespace hector

#endif
