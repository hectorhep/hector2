#include <boost/python.hpp>
#include <boost/python/overloads.hpp>

#include "Core/ExceptionType.h"
#include "Core/Parameters.h"

#include "Propagator/Particle.h"
#include "Propagator/StateVector.h"

BOOST_PYTHON_MODULE( pyhector )
{
  boost::python::enum_<Hector::ExceptionType>( "exceptionType" )
    .value( "undefined", Hector::ExceptionType::Undefined )
    .value( "info", Hector::ExceptionType::Info )
    .value( "justWarning", Hector::ExceptionType::JustWarning )
    .value( "fatal", Hector::ExceptionType::Fatal )
  ;

  boost::python::class_<Hector::Parameters>( "parameters" )
    .def( "beam_energy", &Hector::Parameters::beamEnergy ).staticmethod( "beam_energy" )
    .def( "beam_particles_mass", &Hector::Parameters::beamParticlesMass ).staticmethod( "beam_particles_mass" )
    .def( "crossing_angle_x", &Hector::Parameters::crossingAngleX ).staticmethod( "crossing_angle_x" )
    .def( "crossing_angle_y", &Hector::Parameters::crossingAngleX ).staticmethod( "crossing_angle_y" )
    .def( "beam_particles_charge", &Hector::Parameters::beamParticlesCharge ).staticmethod( "beam_particles_charge" )
    .def( "logging_threshold", &Hector::Parameters::loggingThreshold ).staticmethod( "logging_threshold" )
    .def( "use_relative_energy", &Hector::Parameters::useRelativeEnergy ).staticmethod( "use_relative_energy" )
    .def( "correct_beamline_overlaps", &Hector::Parameters::correctBeamlineOverlaps ).staticmethod( "correct_beamline_overlaps" )
    .def( "compute_aperture_acceptance", &Hector::Parameters::computeApertureAcceptance ).staticmethod( "compute_aperture_acceptance" )
    .def( "enable_kickers", &Hector::Parameters::enableKickers ).staticmethod( "enable_kickers" )
    .def( "enable_dipoles", &Hector::Parameters::enableDipoles ).staticmethod( "enable_dipoles" )
  ;

  boost::python::class_<Hector::Particle>( "particle" )
    .def( "pdgId", &Hector::Particle::pdgId )
    .def( "momentumAt", &Hector::Particle::momentumAt )
  ;

  boost::python::class_<Hector::StateVector>( "stateVector" )
    .def( "energy", &Hector::StateVector::energy )
    .def( "xi", &Hector::StateVector::xi )
  ;
}

