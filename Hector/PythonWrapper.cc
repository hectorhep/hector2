#include <boost/python.hpp>
#include <boost/python/overloads.hpp>
#include <boost/shared_ptr.hpp>

#include <CLHEP/Vector/LorentzVector.h>

#include "Core/ExceptionType.h"
#include "Core/Parameters.h"

#include "Propagator/Particle.h"
#include "Propagator/StateVector.h"

BOOST_PYTHON_MODULE( pyhector )
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
    .def( "beam_energy", &Hector::Parameters::beamEnergy )
    .def( "beam_particles_mass", &Hector::Parameters::beamParticlesMass )
    .def( "crossing_angle_x", &Hector::Parameters::crossingAngleX )
    .def( "crossing_angle_y", &Hector::Parameters::crossingAngleX )
    .def( "beam_particles_charge", &Hector::Parameters::beamParticlesCharge )
    .def( "logging_threshold", &Hector::Parameters::loggingThreshold )
    .def( "use_relative_energy", &Hector::Parameters::useRelativeEnergy )
    .def( "correct_beamline_overlaps", &Hector::Parameters::correctBeamlineOverlaps )
    .def( "compute_aperture_acceptance", &Hector::Parameters::computeApertureAcceptance )
    .def( "enable_kickers", &Hector::Parameters::enableKickers )
    .def( "enable_dipoles", &Hector::Parameters::enableDipoles )
  ;

  boost::python::class_<Hector::Particle>( "particle" )
    .def( "pdgId", &Hector::Particle::pdgId )
    .def( "momentumAt", &Hector::Particle::momentumAt )
  ;

  boost::python::class_<Hector::StateVector>( "stateVector" )
    .def( "energy", &Hector::StateVector::energy )
    .def( "xi", &Hector::StateVector::xi )
    .def( "momentum", &Hector::StateVector::momentum )
  ;
}
