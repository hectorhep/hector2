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
        Parameter( std::string name, std::string description, std::string* var = 0 ) :
          name( name ), description( description ), value( "" ), str_variable( var ), float_variable( 0 ), int_variable( 0 ), uint_variable( 0 ) {}
        Parameter( std::string name, std::string description = "", std::string value = "", std::string* var = 0 ) :
          name( name ), description( description ), value( value ), str_variable( var ), float_variable( 0 ), int_variable( 0 ), uint_variable( 0 ) {}
        Parameter( std::string name, std::string description, unsigned int default_value, unsigned int* var = 0 ) :
          name( name ), description( description ), value( std::to_string( default_value ) ), str_variable( 0 ), float_variable( 0 ), int_variable( 0 ), uint_variable( var ) {}
        Parameter( std::string name, std::string description, int default_value, int* var = 0 ) :
          name( name ), description( description ), value( std::to_string( default_value ) ), str_variable( 0 ), float_variable( 0 ), int_variable( var ), uint_variable( 0 ) {}
        Parameter( std::string name, std::string description, double default_value, double* var = 0 ) :
          name( name ), description( description ), value( std::to_string( default_value ) ), str_variable( 0 ), float_variable( var ), int_variable( 0 ), uint_variable( 0 ) {}
        std::string name, description, value;
        std::string* str_variable;
        double* float_variable;
        int* int_variable;
        unsigned int* uint_variable;
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

