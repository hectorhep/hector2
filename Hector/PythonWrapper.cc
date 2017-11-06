#include <boost/python.hpp>
#include <boost/python/overloads.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/python/return_internal_reference.hpp>

#include <CLHEP/Vector/LorentzVector.h>

#include "Core/ExceptionType.h"
#include "Core/Parameters.h"

#include "Propagator/Particle.h"
#include "Propagator/StateVector.h"

#include "Beamline/Beamline.h"
#include "IO/MADXParser.h"

BOOST_PYTHON_MODULE( pyhector )
{
  namespace py = boost::python;
/*  py::class_<CLHEP::HepLorentzVector>( "lorentzVector" )
    .def( "px", &CLHEP::HepLorentzVector::px )
    .def( "py", &CLHEP::HepLorentzVector::py )
    .def( "pz", &CLHEP::HepLorentzVector::pz )
    .def( "e", &CLHEP::HepLorentzVector::e )
  ;

  py::enum_<Hector::ExceptionType>( "exceptionType" )
    .value( "undefined", Hector::ExceptionType::Undefined )
    .value( "info", Hector::ExceptionType::Info )
    .value( "justWarning", Hector::ExceptionType::JustWarning )
    .value( "fatal", Hector::ExceptionType::Fatal )
  ;*/

  py::class_<Hector::Parameters, std::shared_ptr<Hector::Parameters>, boost::noncopyable>( "parameters", py::no_init )
    .def( "get", &Hector::Parameters::get ).staticmethod( "get" )
    .def( "beamEnergy", &Hector::Parameters::beamEnergy, py::return_value_policy<py::return_by_value>() )
    .def( "beamParticlesMass", &Hector::Parameters::beamParticlesMass, py::return_value_policy<py::return_by_value>() )
    .def( "crossingAngleX", &Hector::Parameters::crossingAngleX, py::return_value_policy<py::return_by_value>() )
    .def( "crossingAngleY", &Hector::Parameters::crossingAngleX, py::return_value_policy<py::return_by_value>() )
    .def( "beamParticlesCharge", &Hector::Parameters::beamParticlesCharge, py::return_value_policy<py::return_by_value>() )
    .def( "loggingThreshold", &Hector::Parameters::loggingThreshold, py::return_value_policy<py::return_by_value>() )
    .def( "useRelativeEnergy", &Hector::Parameters::useRelativeEnergy, py::return_value_policy<py::return_by_value>() )
    .def( "correctBeamlineOverlaps", &Hector::Parameters::correctBeamlineOverlaps, py::return_value_policy<py::return_by_value>() )
    .def( "computeApertureAcceptance", &Hector::Parameters::computeApertureAcceptance, py::return_value_policy<py::return_by_value>() )
    .def( "enableKickers", &Hector::Parameters::enableKickers, py::return_value_policy<py::return_by_value>() )
    .def( "enableDipoles", &Hector::Parameters::enableDipoles, py::return_value_policy<py::return_by_value>() )
  ;

  py::class_<Hector::Particle>( "particle" )
    .def( "pdgId", &Hector::Particle::pdgId )
    .def( "momentumAt", &Hector::Particle::momentumAt )
    .def( "stateVectorAt", &Hector::Particle::stateVectorAt )
  ;

  py::class_<Hector::StateVector>( "stateVector" )
    .def( "energy", &Hector::StateVector::energy )
    .def( "xi", &Hector::StateVector::xi )
    .def( "momentum", &Hector::StateVector::momentum )
  ;

/*  py::class_<Hector::Beamline>( "beamline" )
    //.def( "sequencedBeamline", &Hector::Beamline::sequencedBeamline ).staticmethod( "sequencedBeamline" )
    .def( "clear", &Hector::Beamline::clear )
    .def( "addElement", &Hector::Beamline::addElement )
  ;

  py::class_<Hector::Parser::MADX>( "madXparser", py::init<const char*,const char*,int,py::optional<float> >() )
    .def( "beamline", &Hector::Parser::MADX::beamline )
    .def( "romanPots", &Hector::Parser::MADX::romanPots )
  ;*/
}
