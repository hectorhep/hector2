/*
 *  Hector: a beamline propagation tool
 *  Copyright (C) 2016-2023  Laurent Forthomme
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef Hector_Utils_ArgsParser_h
#define Hector_Utils_ArgsParser_h

#include <string>
#include <vector>

namespace hector {
  /// A generic command line arguments parser
  /// \author Laurent Forthomme
  /// \date Nov 2017
  class ArgsParser {
  public:
    /// A parameter parsed from user's input
    struct Parameter {
      //----- parameters constructors

      //--- string

      /// An optional string parameter
      Parameter(std::string name,
                std::string description = "",
                std::string default_value = "",
                std::string* var = nullptr,
                char sname = '\0');
      /// A string parameter
      Parameter(std::string name, std::string description, std::string* var = nullptr, char sname = '\0');
      /// Smallest string parameter
      Parameter(std::string name, char sname = '\0') : Parameter(name, "", (std::string*)nullptr, sname) {}

      //----- unsigned/signed integers

      /// An optional unsigned integer parameter
      Parameter(std::string name,
                std::string description,
                unsigned int default_value,
                unsigned int* var = nullptr,
                char sname = '\0');
      /// An unsigned integer parameter
      Parameter(std::string name, std::string description, unsigned int* var = nullptr, char sname = '\0');
      /// An optional integer parameter
      Parameter(std::string name, std::string description, int default_value, int* var = nullptr, char sname = '\0');
      /// An integer parameter
      Parameter(std::string name, std::string description, int* var = nullptr, char sname = '\0');
      /// An optional boolean parameter
      Parameter(std::string name, std::string description, bool default_value, bool* var = nullptr, char sname = '\0');
      /// A boolean parameter
      Parameter(std::string name, std::string description, bool* var = nullptr, char sname = '\0');

      //--- floats

      /// An optional double-precision floating point parameter
      Parameter(
          std::string name, std::string description, double default_value, double* var = nullptr, char sname = '\0');
      /// A double-precision floating point parameter
      Parameter(std::string name, std::string description, double* var = nullptr, char sname = '\0');

      //--- complex formats

      /// An optional vector of strings parameter
      Parameter(std::string name,
                std::string description,
                std::vector<std::string> default_value,
                std::vector<std::string>* var = nullptr,
                char sname = '\0');
      /// A vector of strings parameter
      Parameter(std::string name, std::string description, std::vector<std::string>* var = nullptr, char sname = '\0');
      /// An optional vector of integer parameter
      Parameter(std::string name,
                std::string description,
                std::vector<int> default_value,
                std::vector<int>* var = nullptr,
                char sname = '\0');
      /// A vector of integer parameter
      Parameter(std::string name, std::string description, std::vector<int>* var = nullptr, char sname = '\0');
      /// An optional vector of floating point parameter
      Parameter(std::string name,
                std::string description,
                std::vector<double> default_value,
                std::vector<double>* var = nullptr,
                char sname = '\0');
      /// A vector of floating point parameter
      Parameter(std::string name, std::string description, std::vector<double>* var = nullptr, char sname = '\0');

      void parse();

      //----- parameters attributes

      /// Computer-readable name
      std::string name;
      /// Short computer-readable name
      char sname;
      /// User-friendly parameter description
      std::string description;
      /// Value (or default value)
      std::string value;

      //----- parameters containers

      /// Pointer to a string variable possibly handled by this parameter
      std::string* str_variable;
      /// Pointer to a double-precision floating point variable possibly handled by this parameter
      double* float_variable;
      /// Pointer to an integer variable possibly handled by this parameter
      int* int_variable;
      /// Pointer to an unsigned integer variable possibly handled by this parameter
      unsigned int* uint_variable;
      /// Pointer to a boolean variable possibly handled by this parameter
      bool* bool_variable;
      /// Pointer to a vector of string variables possibly handled by this parameter
      std::vector<std::string>* vec_str_variable;
      /// Pointer to a vector of integer variables possibly handled by this parameter
      std::vector<int>* vec_int_variable;
      /// Pointer to a vector of floating point variables possibly handled by this parameter
      std::vector<double>* vec_float_variable;
    };
    /// A collection of parameters
    typedef std::vector<Parameter> ParametersList;

  public:
    /// Arguments parser constructor
    /// \param[in] argc Number of argument words (key + value) to be parsed
    /// \param[in] argv[] List of argument words (key + value) to be parsed
    /// \param[in] required_parameters Collection of parameters required for the parser
    /// \param[in] optional_parameters Collection of non-mandatory parameters that can be parsed
    ArgsParser(int argc,
               char* argv[],
               const ParametersList& required_parameters,
               const ParametersList& optional_parameters = ParametersList());
    /// Read required and optional parameters
    std::string operator[](std::string name) const;
    /// Show usage
    void print_help() const;

  private:
    std::string command_name_;
    const ParametersList help_str_;
    const ParametersList debug_str_;
    ParametersList required_params_, optional_params_;
    std::vector<std::string> args_;
  };
}  // namespace hector

#endif
