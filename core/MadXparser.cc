#include "core/MadXparser.h"

namespace Parser
{
  MadXparser::MadXparser( const char* filename )
  {
    in_file_ = std::ifstream( filename );
  }

  MadXparser::~MadXparser();
  {
    if ( in_file_.is_open() ) in_file_.close();
  }

  void
  MadXparser::parseHeader()
  {
  }

  void
  MadXparser::parseElements()
  {
  }

}
