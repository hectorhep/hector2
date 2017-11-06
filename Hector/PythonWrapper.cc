#include <boost/python.hpp>
#include <boost/python/overloads.hpp>
#include <boost/python/call.hpp>
#include <boost/python/return_internal_reference.hpp>

#include <CLHEP/Vector/LorentzVector.h>

#include "Core/ExceptionType.h"
#include "Core/Parameters.h"

#include "Propagator/Particle.h"
#include "Propagator/StateVector.h"

#include "Beamline/Beamline.h"
#include "Elements/ElementBase.h"
#include "Elements/Drift.h"
#include "Elements/Dipole.h"
#include "Elements/Quadrupole.h"
#include "Elements/Kicker.h"

#include "IO/MADXParser.h"

#include <memory>

namespace py = boost::python;

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
  std::string dump_beamline( const Hector::Beamline& bl ) {
    std::ostringstream os;
    bl.dump( os, true );
    return os.str();
  }
  template<class T>
  void convertElementBase( const char* name )
  {
    struct Wrap : T, py::wrapper<T>
    {
      T* clone() const { return this->get_override( "clone" )(); }
      CLHEP::HepMatrix matrix( float eloss, float mp, int qp ) const { return this->get_override( "matrix" )( eloss, mp, qp ); }
    };
    py::class_<Wrap, boost::noncopyable, py::bases<Hector::Element::ElementBase> >( name, py::no_init )
      .def( "clone", py::pure_virtual( &Hector::Element::ElementBase::clone ), py::return_value_policy<py::manage_new_object>() )
      .def( "matrix", py::pure_virtual( &Hector::Element::ElementBase::matrix ) )
    ;
  }
  template<class T,class init=py::init<std::string,float,float,float>,class P=Hector::Element::ElementBase>
  void convertElement( const char* name )
  {
    py::class_<T, py::bases<P> >( name, init() )
      .def( "clone", &T::clone, py::return_value_policy<py::manage_new_object>() )
      .def( "matrix", &T::matrix )
    ;
  }
}

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( particle_add_position_pos_overloads, addPosition, 1, 2 )
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( particle_add_position_vec_overloads, addPosition, 2, 3 )
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( beamline_dump_overloads, dump, 0, 2 )

BOOST_PYTHON_MODULE( pyhector )
{
  //----- GENERAL HELPERS

  py::class_<CLHEP::HepLorentzVector>( "LorentzVector" )
    .def( "px", &CLHEP::HepLorentzVector::px )
    .def( "py", &CLHEP::HepLorentzVector::py )
    .def( "pz", &CLHEP::HepLorentzVector::pz )
    .def( "e", &CLHEP::HepLorentzVector::e )
  ;

  py::enum_<Hector::ExceptionType>( "ExceptionType" )
    .value( "undefined", Hector::ExceptionType::Undefined )
    .value( "info", Hector::ExceptionType::Info )
    .value( "justWarning", Hector::ExceptionType::JustWarning )
    .value( "fatal", Hector::ExceptionType::Fatal )
  ;

  py::class_<Hector::Parameters, std::shared_ptr<Hector::Parameters>, boost::noncopyable>( "Parameters", py::init<>() )
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

  //----- BEAM PROPERTIES

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

  //----- BEAMLINE DEFINITION

  struct ElementBaseWrap : Hector::Element::ElementBase, py::wrapper<Hector::Element::ElementBase>
  {
    ElementBaseWrap() : Hector::Element::ElementBase( Hector::Element::anInvalidElement ) {}
    Hector::Element::ElementBase* clone() const override { return this->get_override( "clone" )(); }
    CLHEP::HepMatrix matrix( float eloss, float mp, int qp ) const override { return this->get_override( "matrix" )( eloss, mp, qp ); }
  };
  //py::class_<Hector::Element::ElementBase, boost::noncopyable>( "Element", py::no_init )
  py::class_<ElementBaseWrap, boost::noncopyable>( "Element", py::init<>() )
    //.def( "clone", py::pure_virtual( &Hector::Element::ElementBase::clone ), py::return_value_policy<py::reference_existing_object>() )
    .def( "clone", py::pure_virtual( &Hector::Element::ElementBase::clone ), py::return_value_policy<py::manage_new_object>() )
    //.def( "clone", py::pure_virtual( &Hector::Element::ElementBase::clone ), py::return_internal_reference<>() )
    //.def( "clone", py::pure_virtual( &Hector::Element::ElementBase::clone ) )
    .def( "matrix", py::pure_virtual( &Hector::Element::ElementBase::matrix ) )
    .def( "__str__", &Hector::Element::ElementBase::name )
    .add_property( "name", &Hector::Element::ElementBase::name, &Hector::Element::ElementBase::setName )
    .add_property( "type", &Hector::Element::ElementBase::type, &Hector::Element::ElementBase::setType )
    .add_property( "s", &Hector::Element::ElementBase::s, &Hector::Element::ElementBase::setS )
    .add_property( "length", &Hector::Element::ElementBase::length, &Hector::Element::ElementBase::setLength )
    .add_property( "position", &Hector::Element::ElementBase::position, &Hector::Element::ElementBase::setPosition )
    .add_property( "angles", &Hector::Element::ElementBase::angles, &Hector::Element::ElementBase::setAngles )
    .add_property( "magneticStrength", &Hector::Element::ElementBase::magneticStrength, &Hector::Element::ElementBase::setMagneticStrength )
    .add_property( "beta", &Hector::Element::ElementBase::beta, &Hector::Element::ElementBase::setBeta )
    .add_property( "dispersion", &Hector::Element::ElementBase::dispersion, &Hector::Element::ElementBase::setDispersion )
    .add_property( "relativePosition", &Hector::Element::ElementBase::relativePosition, &Hector::Element::ElementBase::setRelativePosition )
  ;

  //--- passive elements
  convertElement<Hector::Element::Drift, py::init<std::string,float,float> >( "Drift" );
  convertElement<Hector::Element::Marker, py::init<std::string,float,float> >( "Marker" );

  //--- dipoles
  convertElementBase<Hector::Element::Dipole>( "Dipole" );
  convertElement<Hector::Element::SectorDipole, py::init<std::string,float,float,float>, Hector::Element::Dipole>( "SectorDipole" );
  convertElement<Hector::Element::RectangularDipole, py::init<std::string,float,float,float>, Hector::Element::Dipole>( "RectangularDipole" );

  //--- quadrupoles
  convertElementBase<Hector::Element::Quadrupole>( "Quadrupole" );
  convertElement<Hector::Element::HorizontalQuadrupole, py::init<std::string,float,float,float>, Hector::Element::Quadrupole>( "HorizontalQuadrupole" );
  convertElement<Hector::Element::VerticalQuadrupole, py::init<std::string,float,float,float>, Hector::Element::Quadrupole>( "VerticalQuadrupole" );

  //--- kickers
  convertElementBase<Hector::Element::Kicker>( "Kicker" );
  convertElement<Hector::Element::HorizontalKicker, py::init<std::string,float,float,float>, Hector::Element::Kicker>( "HorizontalKicker" );
  convertElement<Hector::Element::VerticalKicker, py::init<std::string,float,float,float>, Hector::Element::Kicker>( "VerticalKicker" );

  py::class_<Hector::Beamline>( "Beamline" )
    .def( "__str__", &dump_beamline )
    .def( "dump", &Hector::Beamline::dump, beamline_dump_overloads() )
    //.def( "elements", py::range( &Hector::Beamline::begin, &Hector::Beamline::end ) )
    .def( "sequencedBeamline", &Hector::Beamline::sequencedBeamline ).staticmethod( "sequencedBeamline" )
    .def( "clear", &Hector::Beamline::clear )
    .def( "addElement", &Hector::Beamline::addElement )
  ;

  //----- I/O HANDLERS

  py::class_<Hector::Parser::MADX>( "MadXparser", py::init<const char*,const char*,int,py::optional<float> >() )
    .def( "beamline", &Hector::Parser::MADX::beamline, py::return_value_policy<py::reference_existing_object>() )
    .def( "romanPots", &Hector::Parser::MADX::romanPots )
  ;
}
