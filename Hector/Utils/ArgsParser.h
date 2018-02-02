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

        //----- string

        /// A string parameter
        Parameter( std::string name, std::string description, std::string* var = nullptr, char sname = '\0' );
        /// An optional string parameter
        Parameter( std::string name, std::string description = "", std::string value = "", std::string* var = nullptr, char sname = '\0' );
        /// Smallest string parameter
        Parameter( std::string name, char sname = '\0' ) :
          Parameter( name, "", ( std::string* )nullptr, sname ) {}

        //----- integers

        /// An unsigned integer parameter
        Parameter( std::string name, std::string description, unsigned int* var = nullptr, char sname = '\0' );
        /// An optional unsigned integer parameter
        Parameter( std::string name, std::string description, unsigned int default_value, unsigned int* var = nullptr, char sname = '\0' );
        /// An integer parameter
        Parameter( std::string name, std::string description, int* var = nullptr, char sname = '\0' );
        /// An optional integer parameter
        Parameter( std::string name, std::string description, int default_value, int* var = nullptr, char sname = '\0' );

        //----- floats

        /// A double-precision floating point parameter
        Parameter( std::string name, std::string description, double* var = nullptr, char sname = '\0' );
        /// An optional double-precision floating point parameter
        Parameter( std::string name, std::string description, double default_value, double* var = nullptr, char sname = '\0' );

        //----- complex formats

        /// A vector of strings parameter
        Parameter( std::string name, std::string description, std::vector<std::string>* var = nullptr, char sname = '\0' );
        /// An optional vector of strings parameter
        Parameter( std::string name, std::string description, std::vector<std::string> default_value, std::vector<std::string>* var = nullptr, char sname = '\0');

        /// Computer-readable name
        std::string name;
        /// Short computer-readable name
        char sname;
        /// User-friendly parameter description
        std::string description;
        /// Value (or default value)
        std::string value;
        /// Pointer to a string variable possibly handled by this parameter
        std::string* str_variable;
        /// Pointer to a double-precision floating point variable possibly handled by this parameter
        double* float_variable;
        /// Pointer to an integer variable possibly handled by this parameter
        int* int_variable;
        /// Pointer to an unsigned integer variable possibly handled by this parameter
        unsigned int* uint_variable;
        /// Pointer to a vector of string variables possibly handled by this parameter
        std::vector<std::string>* vec_str_variable;
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

