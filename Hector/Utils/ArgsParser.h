#ifndef Hector_Utils_ArgsParser_h
#define Hector_Utils_ArgsParser_h

#include <vector>
#include <string>
#include <map>

namespace Hector
{
  class ArgsParser
  {
    public:
      struct Parameter {
        Parameter( std::string name, std::string description = "", std::string value = "" ) :
          name( name ), description( description ), value( value ) {}
        Parameter( std::string name, std::string description, int default_value ) :
          name( name ), description( description ), value( std::to_string( default_value ) ) {}
        Parameter( std::string name, std::string description, double default_value ) :
          name( name ), description( description ), value( std::to_string( default_value ) ) {}
        std::string name, description, value;
      };
      typedef std::vector<Parameter> ParametersList;
      typedef std::map<std::string,std::string> ParametersMap;

    public:
      ArgsParser( int argc, char* argv[], const ParametersList& required_parameters, const ParametersList& optional_parameters = ParametersList() );
      /// Read required and optional parameters
      std::string operator[]( std::string name ) const;
      /// Show usage
      void print_help() const;

    private:
      std::string command_name_;
      const ParametersList help_str_;
      ParametersList required_params_, optional_params_;
      std::vector<std::string> args_;
  };
}

#endif

