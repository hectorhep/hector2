#include <boost/python.hpp>
#include <boost/python/overloads.hpp>
#include <boost/python/return_internal_reference.hpp>

#include <CLHEP/Vector/LorentzVector.h>

#include "Core/ExceptionType.h"
#include "Core/Parameters.h"

#include "Propagator/Particle.h"
#include "Propagator/StateVector.h"

#include "Beamline/Beamline.h"
#include "IO/MADXParser.h"

namespace
{
  std::string dump_particle( const Hector::Particle& part ) {
    std::ostringstream os;
    part.dump( os );
    return os.str();
  }
  std::string dump_statevector( const Hector::StateVector& sv ) {
    std::ostringstream os; os << sv;
    return os.str();
  }
}

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( particle_add_position_pos_overloads, addPosition, 1, 2 )
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( particle_add_position_vec_overloads, addPosition, 2, 3 )

BOOST_PYTHON_MODULE( pyhector )
{
  namespace py = boost::python;
  py::class_<CLHEP::HepLorentzVector>( "LorentzVector" )
    .def( "px", &CLHEP::HepLorentzVector::px )
    .def( "py", &CLHEP::HepLorentzVector::py )
    .def( "pz", &CLHEP::HepLorentzVector::pz )
    .def( "e", &CLHEP::HepLorentzVector::e )
  ;

  /*py::enum_<Hector::ExceptionType>( "ExceptionType" )
    .value( "undefined", Hector::ExceptionType::Undefined )
    .value( "info", Hector::ExceptionType::Info )
    .value( "justWarning", Hector::ExceptionType::JustWarning )
    .value( "fatal", Hector::ExceptionType::Fatal )
  ;*/

  py::class_<Hector::Parameters, std::shared_ptr<Hector::Parameters>, boost::noncopyable>( "Parameters", py::no_init )
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

  void ( Hector::Particle::*addPosition_vec )( double, const Hector::StateVector&, bool ) = &Hector::Particle::addPosition;
  void ( Hector::Particle::*addPosition_pos )( const Hector::Particle::Position&, bool ) = &Hector::Particle::addPosition;
  py::class_<Hector::Particle>( "Particle" )
    .def( py::init<CLHEP::HepLorentzVector,int>() )
    .def( py::init<Hector::StateVector,double>() )
    .def( "__str__", &dump_particle )
    .add_property( "pdgId", &Hector::Particle::pdgId, &Hector::Particle::setPDGid )
    .add_property( "charge", &Hector::Particle::charge, &Hector::Particle::setCharge )
    .def( "clear", &Hector::Particle::clear )
    .def( "momentumAt", &Hector::Particle::momentumAt )
    .def( "stateVectorAt", &Hector::Particle::stateVectorAt )
    .def( "addPosition", addPosition_pos, particle_add_position_pos_overloads() )
    .def( "addPosition", addPosition_vec, particle_add_position_vec_overloads() )
  ;

  py::class_<Hector::StateVector>( "StateVector" )
    .def( py::init<CLHEP::HepVector,double>() )
    .def( py::init<CLHEP::HepLorentzVector,CLHEP::Hep2Vector>() )
    .def( py::init<CLHEP::Hep2Vector,CLHEP::Hep2Vector,double,double>() )
    .def( "__str__", &dump_statevector )
    .add_property( "energy", &Hector::StateVector::energy, &Hector::StateVector::setEnergy )
    .add_property( "xi", &Hector::StateVector::xi, &Hector::StateVector::setXi )
    .add_property( "momentum", &Hector::StateVector::momentum, &Hector::StateVector::setMomentum )
  ;

/*  py::class_<Hector::Beamline>( "Beamline" )
    //.def( "sequencedBeamline", &Hector::Beamline::sequencedBeamline ).staticmethod( "sequencedBeamline" )
    .def( "clear", &Hector::Beamline::clear )
    .def( "addElement", &Hector::Beamline::addElement )
  ;

  py::class_<Hector::Parser::MADX>( "MadXparser", py::init<const char*,const char*,int,py::optional<float> >() )
    .def( "beamline", &Hector::Parser::MADX::beamline )
    .def( "romanPots", &Hector::Parser::MADX::romanPots )
  ;*/
}
