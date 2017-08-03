#include <boost/python.hpp>
#include <boost/python/overloads.hpp>
#include <boost/shared_ptr.hpp>

#include <CLHEP/Vector/LorentzVector.h>

#include "Core/ExceptionType.h"
#include "Core/Parameters.h"

#include "Propagator/Particle.h"
#include "Propagator/StateVector.h"

#include "Beamline/Beamline.h"
#include "IO/MADXParser.h"

BOOST_PYTHON_MODULE( libpyhector )
{
  boost::python::class_<CLHEP::HepLorentzVector>( "lorentzVector" )
    .def( "px", &CLHEP::HepLorentzVector::px )
    .def( "py", &CLHEP::HepLorentzVector::py )
    .def( "pz", &CLHEP::HepLorentzVector::pz )
    .def( "e", &CLHEP::HepLorentzVector::e )
  ;

  boost::python::enum_<Hector::ExceptionType>( "exceptionType" )
    .value( "undefined", Hector::ExceptionType::Undefined )
    .value( "info", Hector::ExceptionType::Info )
    .value( "justWarning", Hector::ExceptionType::JustWarning )
    .value( "fatal", Hector::ExceptionType::Fatal )
  ;

  boost::python::class_<Hector::Parameters, std::shared_ptr<Hector::Parameters> >( "parameters", boost::python::init<>() )
    .def( "get", &Hector::Parameters::get ).staticmethod( "get" )
    .def( "beamEnergy", &Hector::Parameters::beamEnergy )
    .def( "beamParticlesMass", &Hector::Parameters::beamParticlesMass )
    .def( "crossingAngleX", &Hector::Parameters::crossingAngleX )
    .def( "crossingAngleY", &Hector::Parameters::crossingAngleX )
    .def( "beamParticlesCharge", &Hector::Parameters::beamParticlesCharge )
    .def( "loggingThreshold", &Hector::Parameters::loggingThreshold )
    .def( "useRelativeEnergy", &Hector::Parameters::useRelativeEnergy )
    .def( "correctBeamlineOverlaps", &Hector::Parameters::correctBeamlineOverlaps )
    .def( "computeApertureAcceptance", &Hector::Parameters::computeApertureAcceptance )
    .def( "enableKickers", &Hector::Parameters::enableKickers )
    .def( "enableDipoles", &Hector::Parameters::enableDipoles )
  ;

  boost::python::class_<Hector::Particle>( "particle" )
    .def( "pdgId", &Hector::Particle::pdgId )
    .def( "momentumAt", &Hector::Particle::momentumAt )
    .def( "stateVectorAt", &Hector::Particle::stateVectorAt )
  ;

  boost::python::class_<Hector::StateVector>( "stateVector" )
    .def( "energy", &Hector::StateVector::energy )
    .def( "xi", &Hector::StateVector::xi )
    .def( "momentum", &Hector::StateVector::momentum )
  ;

  boost::python::class_<Hector::Beamline>( "beamline" )
    //.def( "sequencedBeamline", &Hector::Beamline::sequencedBeamline ).staticmethod( "sequencedBeamline" )
    .def( "clear", &Hector::Beamline::clear )
    .def( "addElement", &Hector::Beamline::addElement )
  ;

  boost::python::class_<Hector::Parser::MADX>( "madXparser", boost::python::init<const char*,const char*,int,boost::python::optional<float> >() )
    .def( "beamline", &Hector::Parser::MADX::beamline )
    .def( "romanPots", &Hector::Parser::MADX::romanPots )
  ;
}
