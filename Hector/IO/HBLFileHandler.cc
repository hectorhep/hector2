#include "Hector/IO/HBLFileHandler.h"
#include "Hector/Core/Exception.h"

#include <fstream>

namespace Hector
{
  namespace Parser
  {
    HBL::HBL( const char* filename )
    {
      std::ifstream file( filename );
      if ( !file.is_open() )
        throw Exception( __PRETTY_FUNCTION__, Form( "Impossible to open file \"%s\" for reading!", filename ), Fatal );
      //while
    }
  }
}
