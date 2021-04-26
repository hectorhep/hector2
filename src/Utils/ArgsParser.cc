#include "Hector/Utils/ArgsParser.h"
#include "Hector/Utils/String.h"
#include "Hector/Exception.h"

#include <sstream>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <stdexcept>

namespace hector {
  ArgsParser::ArgsParser(int argc,
                         char* argv[],
                         const ParametersList& required_parameters,
                         const ParametersList& optional_parameters)
      : help_str_({{"help", 'h'}}), required_params_(required_parameters), optional_params_(optional_parameters) {
    command_name_ = argv[0];
    std::vector<std::string> args_tmp;
    if (argc > 1) {
      args_tmp.resize(argc - 1);
      std::copy(argv + 1, argv + argc, args_tmp.begin());
    }
    for (const auto& arg : args_tmp) {
      const auto eq_pos = arg.find('=');
      if (eq_pos == std::string::npos) {
        args_.emplace_back(arg);
        continue;
      }
      args_.emplace_back(arg.substr(0, eq_pos));
      args_.emplace_back(arg.substr(eq_pos + 1));
    }
    for (const auto& str : help_str_) {
      if (find(args_.begin(), args_.end(), "--" + str.name) != args_.end() ||
          find(args_.begin(), args_.end(), "-" + std::string(1, str.sname)) != args_.end()) {
        print_help();
        exit(0);
      }
    }
    for (auto& par : required_params_) {
      const auto key = find(args_.begin(), args_.end(), "--" + par.name);
      const auto skey = find(args_.begin(), args_.end(), "-" + std::string(1, par.sname));
      if (key == args_.end() && skey == args_.end()) {
        print_help();
        throw H_ERROR << "The following parameter was not set: " << par.name << ".";
      }
      const auto value = (key != args_.end()) ? key + 1 : skey + 1;
      if (value == args_.end())
        throw H_ERROR << "Invalid value for parameter: " << par.name << ".";

      par.value = *value;
      par.parse();
    }
    for (auto& par : optional_params_) {
      const auto key = find(args_.begin(), args_.end(), "--" + par.name);
      const auto skey = find(args_.begin(), args_.end(), "-" + std::string(1, par.sname));
      if (key != args_.end() || skey != args_.end()) {  // Parameter set
        const auto value = (key != args_.end()) ? key + 1 : skey + 1;
        if (value != args_.end()) {
          for (const auto& par2 : optional_params_)
            if (*value == "--" + par2.name || *value == "-" + std::string(1, par.sname))
              throw H_ERROR << "Invalid value for parameter: " << par.name << ".";
          par.value = *value;
        } else if (par.bool_variable)
          par.value = "1";  // if the flag is set, enabled by default
        else
          throw H_ERROR << "Invalid value for parameter: " << par.name << ".";
      }
      par.parse();
    }
  }

  std::string ArgsParser::operator[](std::string name) const {
    for (const auto& par : required_params_) {
      if ("--" + par.name == name)
        return par.value;
      if (par.sname != '\0' && "-" + std::string(1, par.sname) == name)
        return par.value;
    }
    for (const auto& par : optional_params_) {
      if ("--" + par.name == name)
        return par.value;
      if (par.sname != '\0' && "-" + std::string(1, par.sname) == name)
        return par.value;
    }
    throw H_ERROR << "The parameter \"" << name << "\" was not declared in the arguments parser constructor!";
  }

  void ArgsParser::print_help() const {
    std::ostringstream oss;
    oss << "Usage: " << command_name_;
    for (const auto& par : required_params_) {
      oss << " --" << par.name;
      if (par.sname != '\0')
        oss << "|-" << par.sname;
    }
    for (const auto& par : optional_params_) {
      oss << " [--" << par.name;
      if (par.sname != '\0')
        oss << " | -" << par.sname;
      oss << "]";
    }
    if (required_params_.size() > 0) {
      oss << "\n required argument" << ((required_params_.size() > 1) ? "s" : "") << ":";
      for (const auto& par : required_params_)
        oss << format((par.sname != '\0') ? "\n\t--%-20s -%1s\t%-40s" : "\n\t--%-20s %2s\t%-40s",
                      par.name.c_str(),
                      &par.sname,
                      par.description.c_str());
    }
    if (optional_params_.size() > 0) {
      oss << "\n optional argument" << ((optional_params_.size() > 1) ? "s" : "") << ":";
      for (const auto& par : optional_params_)
        oss << format(
            (par.sname != '\0') ? "\n\t--%-20s -%1s\t%-40s\tdefault = '%s'" : "\n\t--%-20s %2s\t%-40s\tdefault = '%s'",
            par.name.c_str(),
            &par.sname,
            par.description.c_str(),
            par.value.c_str());
    }
    oss << std::endl;
    std::cout << oss.str();
  }

  //----- simple parameters

  ArgsParser::Parameter::Parameter(
      std::string name, std::string description, std::string default_value, std::string* var, char sname)
      : name(name),
        sname(sname),
        description(description),
        value(default_value),
        str_variable(var),
        float_variable(nullptr),
        int_variable(nullptr),
        uint_variable(nullptr),
        bool_variable(nullptr),
        vec_str_variable(nullptr),
        vec_int_variable(nullptr),
        vec_float_variable(nullptr) {}

  ArgsParser::Parameter::Parameter(std::string name, std::string description, std::string* var, char sname)
      : Parameter(name, description, "", var, sname) {}

  ArgsParser::Parameter::Parameter(
      std::string name, std::string description, unsigned int default_value, unsigned int* var, char sname)
      : name(name),
        sname(sname),
        description(description),
        value(std::to_string(default_value)),
        str_variable(nullptr),
        float_variable(nullptr),
        int_variable(nullptr),
        uint_variable(var),
        bool_variable(nullptr),
        vec_str_variable(nullptr),
        vec_int_variable(nullptr),
        vec_float_variable(nullptr) {}

  ArgsParser::Parameter::Parameter(std::string name, std::string description, unsigned int* var, char sname)
      : Parameter(name, description, 0, var, sname) {}

  ArgsParser::Parameter::Parameter(std::string name, std::string description, int default_value, int* var, char sname)
      : name(name),
        sname(sname),
        description(description),
        value(format("%+i", default_value)),
        str_variable(nullptr),
        float_variable(nullptr),
        int_variable(var),
        uint_variable(nullptr),
        bool_variable(nullptr),
        vec_str_variable(nullptr),
        vec_int_variable(nullptr),
        vec_float_variable(nullptr) {}

  ArgsParser::Parameter::Parameter(std::string name, std::string description, int* var, char sname)
      : Parameter(name, description, 0, var, sname) {}

  ArgsParser::Parameter::Parameter(std::string name, std::string description, bool default_value, bool* var, char sname)
      : name(name),
        sname(sname),
        description(description),
        value(format("%d", default_value)),
        str_variable(nullptr),
        float_variable(nullptr),
        int_variable(nullptr),
        uint_variable(nullptr),
        bool_variable(var),
        vec_str_variable(nullptr),
        vec_int_variable(nullptr),
        vec_float_variable(nullptr) {}

  ArgsParser::Parameter::Parameter(std::string name, std::string description, bool* var, char sname)
      : Parameter(name, description, 0, var, sname) {}

  ArgsParser::Parameter::Parameter(
      std::string name, std::string description, double default_value, double* var, char sname)
      : name(name),
        sname(sname),
        description(description),
        value(format("%g", default_value)),
        str_variable(nullptr),
        float_variable(var),
        int_variable(nullptr),
        uint_variable(nullptr),
        bool_variable(nullptr),
        vec_str_variable(nullptr),
        vec_int_variable(nullptr),
        vec_float_variable(nullptr) {}

  ArgsParser::Parameter::Parameter(std::string name, std::string description, double* var, char sname)
      : Parameter(name, description, 0., var, sname) {}

  //----- vector of parameters

  ArgsParser::Parameter::Parameter(std::string name,
                                   std::string description,
                                   std::vector<std::string> default_value,
                                   std::vector<std::string>* var,
                                   char sname)
      : name(name),
        sname(sname),
        description(description),
        value(""),
        str_variable(nullptr),
        float_variable(nullptr),
        int_variable(nullptr),
        uint_variable(nullptr),
        bool_variable(nullptr),
        vec_str_variable(var),
        vec_int_variable(nullptr),
        vec_float_variable(nullptr) {
    unsigned short i = 0;
    for (const auto& str : default_value)
      value += (((i++ > 0) ? "," : "") + str);
  }

  ArgsParser::Parameter::Parameter(std::string name, std::string description, std::vector<std::string>* var, char sname)
      : Parameter(name, description, std::vector<std::string>{{}}, var, sname) {}

  ArgsParser::Parameter::Parameter(
      std::string name, std::string description, std::vector<int> default_value, std::vector<int>* var, char sname)
      : name(name),
        sname(sname),
        description(description),
        value(""),
        str_variable(nullptr),
        float_variable(nullptr),
        int_variable(nullptr),
        uint_variable(nullptr),
        bool_variable(nullptr),
        vec_str_variable(nullptr),
        vec_int_variable(var),
        vec_float_variable(nullptr) {
    unsigned short i = 0;
    for (const auto& val : default_value)
      value += (((i++ > 0) ? "," : "") + format("%d", val));
  }

  ArgsParser::Parameter::Parameter(std::string name, std::string description, std::vector<int>* var, char sname)
      : Parameter(name, description, std::vector<int>{{}}, var, sname) {}

  ArgsParser::Parameter::Parameter(std::string name,
                                   std::string description,
                                   std::vector<double> default_value,
                                   std::vector<double>* var,
                                   char sname)
      : name(name),
        sname(sname),
        description(description),
        value(""),
        str_variable(nullptr),
        float_variable(nullptr),
        int_variable(nullptr),
        uint_variable(nullptr),
        bool_variable(nullptr),
        vec_str_variable(nullptr),
        vec_int_variable(nullptr),
        vec_float_variable(var) {
    unsigned short i = 0;
    for (const auto& flt : default_value)
      value += (((i++ > 0) ? "," : "") + format("%g", flt));
  }

  ArgsParser::Parameter::Parameter(std::string name, std::string description, std::vector<double>* var, char sname)
      : Parameter(name, description, std::vector<double>{{}}, var, sname) {}

  void ArgsParser::Parameter::parse() {
    if (str_variable != nullptr)
      *str_variable = value;
    else if (float_variable != nullptr)
      *float_variable = std::stod(value);
    else if (int_variable != nullptr)
      *int_variable = std::stoi(value);
    else if (uint_variable != nullptr)
      *uint_variable = std::stoi(value);
    else if (bool_variable != nullptr) {
      try {
        *bool_variable = (std::stoi(value) != 0);
      } catch (const std::invalid_argument&) {
        *bool_variable = (strcasecmp("true", value.c_str()) == 0 && strcasecmp("false", value.c_str()) != 0);
      }
    } else if (vec_str_variable != nullptr) {
      std::istringstream iss(value);
      std::string token;
      std::vector<std::string> vec_var;
      while (std::getline(iss, token, ','))
        vec_var.emplace_back(token);
      *vec_str_variable = vec_var;
    } else if (vec_int_variable != nullptr) {
      std::istringstream iss(value);
      std::string token;
      std::vector<int> vec_var;
      while (std::getline(iss, token, ','))
        vec_var.emplace_back(std::stoi(token));
      *vec_int_variable = vec_var;
    } else if (vec_float_variable != nullptr) {
      std::istringstream iss(value);
      std::string token;
      std::vector<double> vec_var;
      while (std::getline(iss, token, ','))
        vec_var.emplace_back(std::stod(token));
      *vec_float_variable = vec_var;
    }
  }
}  // namespace hector
