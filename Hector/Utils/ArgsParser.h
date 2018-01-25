#ifndef Hector_Utils_ArgsParser_h
#define Hector_Utils_ArgsParser_h

#include <vector>
#include <string>
#include <map>

namespace Hector
{
  /// A generic command line arguments parser
  /// \author Laurent Forthomme
  /// \date Nov 2017
  class ArgsParser
  {
    public:
      /// A parameter parsed from user's input
      struct Parameter {
        /// An optional string parameter
        Parameter( std::string name, std::string description = "", std::string value = "", std::string* var = 0 ) :
          name( name ), description( description ), value( value ), str_variable( var ), float_variable( 0 ), int_variable( 0 ), uint_variable( 0 ) {}
        /// A string parameter
        Parameter( std::string name, std::string description, std::string* var = 0 ) :
          name( name ), description( description ), value( "" ), str_variable( var ), float_variable( 0 ), int_variable( 0 ), uint_variable( 0 ) {}
        /// An optional unsigned integer parameter
        Parameter( std::string name, std::string description, unsigned int default_value, unsigned int* var = 0 ) :
          name( name ), description( description ), value( std::to_string( default_value ) ), str_variable( 0 ), float_variable( 0 ), int_variable( 0 ), uint_variable( var ) {}
        /// An unsigned integer parameter
        Parameter( std::string name, std::string description, unsigned int* var = 0 ) :
          name( name ), description( description ), value( std::to_string( 0 ) ), str_variable( 0 ), float_variable( 0 ), int_variable( 0 ), uint_variable( var ) {}
        /// An optional integer parameter
        Parameter( std::string name, std::string description, int default_value, int* var = 0 ) :
          name( name ), description( description ), value( std::to_string( default_value ) ), str_variable( 0 ), float_variable( 0 ), int_variable( var ), uint_variable( 0 ) {}
        /// An integer parameter
        Parameter( std::string name, std::string description, int* var = 0 ) :
          name( name ), description( description ), value( std::to_string( 0 ) ), str_variable( 0 ), float_variable( 0 ), int_variable( var ), uint_variable( 0 ) {}
        /// An optional double-precision floating point parameter
        Parameter( std::string name, std::string description, double default_value, double* var = 0 ) :
          name( name ), description( description ), value( std::to_string( default_value ) ), str_variable( 0 ), float_variable( var ), int_variable( 0 ), uint_variable( 0 ) {}
        /// A double-precision floating point parameter
        Parameter( std::string name, std::string description, double* var = 0 ) :
          name( name ), description( description ), value( std::to_string( 0. ) ), str_variable( 0 ), float_variable( var ), int_variable( 0 ), uint_variable( 0 ) {}
        /// Parameter computer-readable name
        std::string name;
        /// User-friendly parameter description
        std::string description;
        /// Parameter value (or default value)
        std::string value;
        /// Pointer to a string variable possibly handled by this parameter
        std::string* str_variable;
        /// Pointer to a double-precision floating point variable possibly handled by this parameter
        double* float_variable;
        /// Pointer to an integer variable possibly handled by this parameter
        int* int_variable;
        /// Pointer to an unsigned integer variable possibly handled by this parameter
        unsigned int* uint_variable;
      };
      /// A collection of parameters
      typedef std::vector<Parameter> ParametersList;

    public:
      /// Arguments parser constructor
      /// \param[in] argc Number of argument words (key + value) to be parsed
      /// \param[in] argv[] List of argument words (key + value) to be parsed
      /// \param[in] required_parameters Collection of parameters required for the parser
      /// \param[in] optional_parameters Collection of non-mandatory parameters that can be parsed
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

