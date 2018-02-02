#include "Hector/Utils/ArgsParser.h"
#include "Hector/Core/Exception.h"

#include <sstream>
#include <algorithm>

namespace Hector
{
  ArgsParser::ArgsParser( int argc, char* argv[], const ParametersList& required_parameters, const ParametersList& optional_parameters ) :
    help_str_( { { "-h" }, { "--help" } } ),
    required_params_( required_parameters ), optional_params_( optional_parameters )
  {
    command_name_ = argv[0];
    std::vector<std::string> args_tmp;
    if ( argc > 1 ) {
      args_tmp.resize( argc-1 );
      std::copy( argv+1, argv+argc, args_tmp.begin() );
    }
    for ( const auto& arg : args_tmp ) {
      const auto eq_pos = arg.find( '=' );
      if ( eq_pos == std::string::npos ) {
        args_.emplace_back( arg );
        continue;
      }
      args_.emplace_back( arg.substr( 0, eq_pos ) );
      args_.emplace_back( arg.substr( eq_pos+1 ) );
    }
    for ( const auto& str : help_str_ ) {
      if ( find( args_.begin(), args_.end(), str.name ) != args_.end() ) {
        print_help();
        exit( 0 );
      }
    }
    for ( auto& par : required_params_ ) {
      const auto key = find( args_.begin(), args_.end(), par.name );
      if ( key == args_.end() ) {
        print_help();
        throw Exception( __PRETTY_FUNCTION__, Form( "The following parameter was not set: %s", par.name.c_str() ), Fatal );
      }
      const auto value = key + 1;
      if ( value == args_.end() ) {
        throw Exception( __PRETTY_FUNCTION__, Form( "Invalid value for parameter: %s", par.name.c_str() ), Fatal );
      }
      par.value = *value;
      if ( par.str_variable ) *par.str_variable = *value;
      if ( par.float_variable ) *par.float_variable = std::stod( *value );
      if ( par.int_variable ) *par.int_variable = std::stoi( *value );
      if ( par.uint_variable ) *par.uint_variable = std::stoi( *value );
      if ( par.vec_str_variable ) {
        std::istringstream iss( par.value ); std::string token;
        std::vector<std::string> vec_var;
        while ( std::getline( iss, token, ',' ) ) vec_var.emplace_back( token );
        *par.vec_str_variable = vec_var;
      }
    }
    for ( auto& par : optional_params_ ) {
      const auto key = find( args_.begin(), args_.end(), par.name );
      if ( key != args_.end() ) { // Parameter set
        const auto value = key + 1;
        if ( value == args_.end() ) {
          throw Exception( __PRETTY_FUNCTION__, Form( "Invalid value for parameter: %s", par.name.c_str() ), Fatal );
        }
        par.value = *value;
      }
      if ( par.str_variable ) *par.str_variable = par.value;
      if ( par.float_variable ) *par.float_variable = std::stod( par.value );
      if ( par.int_variable ) *par.int_variable = std::stoi( par.value );
      if ( par.uint_variable ) *par.uint_variable = std::stoi( par.value );
      if ( par.vec_str_variable ) {
        std::istringstream iss( par.value ); std::string token;
        std::vector<std::string> vec_var;
        while ( std::getline( iss, token, ',' ) ) vec_var.emplace_back( token );
        *par.vec_str_variable = vec_var;
      }
    }
  }

  std::string
  ArgsParser::operator[]( std::string name ) const
  {
    for ( const auto& par : required_params_ ) {
      if ( par.name == name ) return par.value;
    }
    for ( const auto& par : optional_params_ ) {
      if ( par.name == name ) return par.value;
    }
    throw Exception( __PRETTY_FUNCTION__, Form( "The parameter \"%s\" was not declared in the arguments parser constructor!", name.c_str() ), Fatal );
  }

  void
  ArgsParser::print_help() const
  {
    std::ostringstream oss;
    oss << "Usage: " << command_name_;
    for ( const auto& par : required_params_ ) oss << " " << par.name;
    for ( const auto& par : optional_params_ ) oss << " [" << par.name << "]";
    if ( required_params_.size() > 0 ) {
     oss << "\n required argument" << ( ( required_params_.size() > 1 ) ? "s" : "" ) << ":";
     for ( const auto& par : required_params_ ) {
       oss << "\n\t" << std::left << std::setw( 20 ) << par.name << "\t" << std::setw( 40 ) << par.description << std::right;
     }
    }
    if ( optional_params_.size() > 0 ) {
     oss << "\n optional argument" << ( ( optional_params_.size() > 1 ) ? "s" : "" ) << ":";
     for ( const auto& par : optional_params_ ) {
       oss << "\n\t" << std::left << std::setw( 20 ) << par.name << "\t" << std::setw( 40 ) << par.description << "\tdefault = '" << par.value << "'" << std::right;
     }
    }
    oss << std::endl;
    std::cout << oss.str(); 
  }

  ArgsParser::Parameter::Parameter( std::string name, std::string description, std::string value, std::string* var ) :
    name( name ), description( description ), value( value ),
    str_variable( var ), float_variable( nullptr ),
    int_variable( nullptr ), uint_variable( nullptr ),
    vec_str_variable( nullptr )
  {}

  ArgsParser::Parameter::Parameter( std::string name, std::string description, std::string* var ) :
    name( name ), description( description ), value( "" ),
    str_variable( var ), float_variable( nullptr ),
    int_variable( nullptr ), uint_variable( nullptr ),
    vec_str_variable( nullptr )
  {}

  ArgsParser::Parameter::Parameter( std::string name, std::string description, unsigned int default_value, unsigned int* var ) :
    name( name ), description( description ), value( std::to_string( default_value ) ),
    str_variable( nullptr ), float_variable( nullptr ),
    int_variable( nullptr ), uint_variable( var ),
    vec_str_variable( nullptr )
  {}

  ArgsParser::Parameter::Parameter( std::string name, std::string description, unsigned int* var ) :
    name( name ), description( description ), value( std::to_string( 0 ) ),
    str_variable( nullptr ), float_variable( nullptr ),
    int_variable( nullptr ), uint_variable( var ),
    vec_str_variable( nullptr )
  {}

  ArgsParser::Parameter::Parameter( std::string name, std::string description, int default_value, int* var ) :
    name( name ), description( description ), value( std::to_string( default_value ) ),
    str_variable( nullptr ), float_variable( nullptr ),
    int_variable( var ), uint_variable( nullptr ),
    vec_str_variable( nullptr )
  {}

  ArgsParser::Parameter::Parameter( std::string name, std::string description, int* var ) :
    name( name ), description( description ), value( std::to_string( 0 ) ),
    str_variable( nullptr ), float_variable( nullptr ),
    int_variable( var ), uint_variable( nullptr ),
    vec_str_variable( nullptr )
  {}

  ArgsParser::Parameter::Parameter( std::string name, std::string description, double default_value, double* var ) :
    name( name ), description( description ), value( Form( "%.5e", default_value ) ),
    str_variable( nullptr ), float_variable( var ),
    int_variable( nullptr ), uint_variable( nullptr ),
    vec_str_variable( nullptr )
  {}

  ArgsParser::Parameter::Parameter( std::string name, std::string description, double* var ) :
    name( name ), description( description ), value( std::to_string( 0. ) ),
    str_variable( nullptr ), float_variable( var ),
    int_variable( nullptr ), uint_variable( nullptr ),
    vec_str_variable( nullptr )
  {}

  ArgsParser::Parameter::Parameter( std::string name, std::string description, std::vector<std::string> default_value, std::vector<std::string>* var ) :
    name( name ), description( description ), value( "" ),
    str_variable( nullptr ), float_variable( nullptr ),
    int_variable( nullptr ), uint_variable( nullptr ),
    vec_str_variable( var )
  {
    for ( const auto& str : default_value )
      value += ( ( ( str != default_value.front() ) ? "," : "" )+str );
  }

  ArgsParser::Parameter::Parameter( std::string name, std::string description, std::vector<std::string>* var ) :
    name( name ), description( description ), value( std::to_string( 0 ) ),
    str_variable( nullptr ), float_variable( nullptr ),
    int_variable( nullptr ), uint_variable( nullptr ),
    vec_str_variable( var )
  {}

}

