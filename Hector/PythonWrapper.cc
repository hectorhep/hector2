#include "Hector/Core/Exception.h"
#include "Hector/Core/ParticleStoppedException.h"
#include "Hector/Core/Parameters.h"

#include "Hector/Propagator/Propagator.h"
#include "Hector/Propagator/Particle.h"
#include "Hector/Propagator/StateVector.h"

#include "Hector/Beamline/Beamline.h"

#include "Hector/Elements/ElementBase.h"
#include "Hector/Elements/Drift.h"
#include "Hector/Elements/Dipole.h"
#include "Hector/Elements/Quadrupole.h"
#include "Hector/Elements/Kicker.h"

#include "Hector/Elements/ApertureBase.h"
#include "Hector/Elements/CircularAperture.h"
#include "Hector/Elements/EllipticAperture.h"
#include "Hector/Elements/RectangularAperture.h"
#include "Hector/Elements/RectEllipticAperture.h"

#include "Hector/IO/MADXHandler.h"

#include "Hector/Utils/BeamProducer.h"
#include <CLHEP/Matrix/Matrix.h>
#include <CLHEP/Random/RandGauss.h>

#include <map>
#include <memory>

#include <boost/version.hpp>
#include <boost/config.hpp>

//----- FIRST START WITH SOME UTILITIES

#if BOOST_VERSION < 105300 || defined BOOST_NO_CXX11_SMART_PTR

// https://github.com/mapnik/mapnik/issues/2022
namespace boost {
  template<class T> const T* get_pointer( const std::shared_ptr<T>& p ) { return p.get(); }
  template<class T> T* get_pointer( std::shared_ptr<T>& p ) { return p.get(); }
}
#endif

#include <boost/python.hpp>

namespace
{
  namespace py = boost::python;

  std::string dump_particle( const Hector::Particle& part ) {
    std::ostringstream os;
    part.dump( os );
    return os.str();
  }
  std::string dump_beamline( const Hector::Beamline& bl ) {
    std::ostringstream os;
    bl.dump( os, true );
    return os.str();
  }
  //--- helper python <-> C++ converters
  template<class T, class U> py::dict to_python_dict( std::map<T,U>& map ) { py::dict dictionary; for ( auto& it : map ) dictionary[it.first] = it.second; return dictionary; }
  template<class T> py::list to_python_list( std::vector<T>& vec ) { py::list list; for ( auto& it : vec ) list.append( it ); return list; }
  template<class T> struct PyMap {
    PyMap() {}
    PyMap( py::dict& dict, std::vector<std::string> keys = std::vector<std::string>{} ) {
      for ( const auto& k : keys ) map[k] = T();
      update_map( dict );
    }
    void clear() { map.clear(); }
    size_t size() const { return map.size(); }
    void update_map( py::dict& dict ) {
      py::list keys = dict.keys();
      // not very C++like, but that's life
      for ( unsigned short i = 0; i < py::len( keys ); ++i ) {
        // extract the key
        py::extract<std::string> extracted_key( keys[i] );
        std::string key = extracted_key;
        if ( !extracted_key.check() ) { std::cerr << "Key invalid, map might be incomplete" << std::endl; continue; }
        // extract the value
        py::extract<T> extracted_val( dict[key] );
        if ( !extracted_val.check() ) { std::cerr << "Value invalid, map might be incomplete" << std::endl; continue; }
        T value = extracted_val;
        // feed the map
        map[key] = value;
      }
    }
    py::dict get_dict() const { py::dict dictionary; for ( auto& it : map ) dictionary[it.first] = it.second; return dictionary; }
    std::map<std::string,T> map;
  };
  py::dict particle_positions( Hector::Particle& part ) { return to_python_dict<double,Hector::StateVector>( part.positions() ); }
  py::list beamline_elements( Hector::Beamline& bl ) { return to_python_list<std::shared_ptr<Hector::Element::ElementBase> >( bl.elements() ); }

  PyObject* except_type = nullptr, *ps_except_type = nullptr;

  void translate_exception( const Hector::Exception& e ) {
    if ( except_type == NULL ) return;
    PyErr_SetObject( except_type, py::object( e ).ptr() );
  }
  void translate_ps_exception( const Hector::ParticleStoppedException& e ) {
    if ( ps_except_type == NULL ) return;
    PyErr_SetObject( ps_except_type, py::object( e ).ptr() );
  }

  template<class T>
  struct BeamProducerWrap : PyMap<float>, T
  {
    BeamProducerWrap() : PyMap<float>(), T() {}
    BeamProducerWrap( py::dict& dict ) :
      PyMap<float>( dict, { { "Emin", "Emax", "smin", "smax", "xmin", "xmax", "ymin", "ymax", "Txmin", "Txmax", "Tymin", "Tymax", "m", "q" } } ),
      T( map["Emin"], map["Emax"], map["smin"], map["smax"], map["xmin"], map["xmax"], map["ymin"], map["ymax"], map["Txmin"], map["Txmax"], map["Tymin"], map["Tymax"], map["m"], map["q"] ) {}
  };
  //--- helper beamline elements definitions
  struct ElementBaseWrap : Hector::Element::ElementBase, py::wrapper<Hector::Element::ElementBase>
  {
    ElementBaseWrap() : Hector::Element::ElementBase( Hector::Element::anInvalidElement ) {}
    std::shared_ptr<Hector::Element::ElementBase> clone() const override { return this->get_override( "clone" )(); }
    Hector::Matrix matrix( float eloss, float mp, int qp ) const override { return this->get_override( "matrix" )( eloss, mp, qp ); }
  };
  template<class T, class init = py::init<std::string,float,float,float> >
  void convertElement( const char* name )
  {
    py::class_<T, py::bases<Hector::Element::ElementBase> >( name, init() )
      .def( "clone", &T::clone, py::return_value_policy<py::return_by_value>() )
      .def( "matrix", &T::matrix )
    ;
  }

  struct ApertureBaseWrap : Hector::Aperture::ApertureBase, py::wrapper<Hector::Aperture::ApertureBase>
  {
    ApertureBaseWrap() : Hector::Aperture::ApertureBase( Hector::Aperture::anInvalidAperture, Hector::TwoVector(), std::vector<float>{} ) {}
    std::shared_ptr<Hector::Aperture::ApertureBase> clone() const override { return this->get_override( "clone" )(); }
  };
  template<class T, class init>
  void convertAperture( const char* name )
  {
    py::class_<T, py::bases<Hector::Aperture::ApertureBase> >( name, init() )
      .def( "clone", &T::clone, py::return_value_policy<py::return_by_value>() )
    ;
  }
}

//----- THEN SOME OVERLOADED FUNCTIONS/METHODS HELPERS

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( particle_add_position_pos_overloads, addPosition, 1, 2 )
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( particle_add_position_vec_overloads, addPosition, 2, 3 )
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( beamline_dump_overloads, dump, 0, 2 )

//----- AND HERE COMES THE MODULE

BOOST_PYTHON_MODULE( pyhector )
{
  //----- GENERAL HELPERS

  py::class_<Hector::TwoVector>( "TwoVector" )
    .def( py::init<double,double>() )
    .def( py::self_ns::str( py::self_ns::self ) )
    .def( py::self += py::other<Hector::TwoVector>() ).def( py::self -= py::other<Hector::TwoVector>() )
    .add_property( "x", &Hector::TwoVector::x, &Hector::TwoVector::setX )
    .add_property( "y", &Hector::TwoVector::y, &Hector::TwoVector::setY )
    .add_property( "mag", &Hector::TwoVector::mag, &Hector::TwoVector::setMag )
    .add_property( "phi", &Hector::TwoVector::phi, &Hector::TwoVector::setPhi )
  ;

  double ( Hector::ThreeVector::*theta_val )() const = &Hector::ThreeVector::theta;
  py::class_<Hector::ThreeVector>( "ThreeVector", "A generic 3-vector for spatial coordinates" )
    .def( py::init<double,double,double>() )
    .def( py::self_ns::str( py::self_ns::self ) )
    .def( py::self += py::other<Hector::ThreeVector>() ).def( py::self -= py::other<Hector::ThreeVector>() )
    .add_property( "x", &Hector::ThreeVector::x, &Hector::ThreeVector::setX )
    .add_property( "y", &Hector::ThreeVector::y, &Hector::ThreeVector::setY )
    .add_property( "z", &Hector::ThreeVector::z, &Hector::ThreeVector::setZ )
    .add_property( "mag", &Hector::ThreeVector::mag, &Hector::ThreeVector::setMag )
    .add_property( "theta", theta_val, &Hector::ThreeVector::setTheta )
    .add_property( "phi", &Hector::ThreeVector::phi, &Hector::ThreeVector::setPhi )
    .def( "deltaR", &Hector::ThreeVector::deltaR )
  ;

  py::class_<Hector::LorentzVector>( "LorentzVector" )
    .def( py::init<double,double,double,double>() )
    .def( py::self_ns::str( py::self_ns::self ) )
    .def( py::self += py::other<Hector::LorentzVector>() ).def( py::self -= py::other<Hector::LorentzVector>() )
    .add_property( "x", &Hector::LorentzVector::x, &Hector::LorentzVector::setX )
    .add_property( "y", &Hector::LorentzVector::y, &Hector::LorentzVector::setY )
    .add_property( "z", &Hector::LorentzVector::z, &Hector::LorentzVector::setZ )
    .add_property( "t", &Hector::LorentzVector::t, &Hector::LorentzVector::setT )
    .add_property( "px", &Hector::LorentzVector::px, &Hector::LorentzVector::setPx )
    .add_property( "py", &Hector::LorentzVector::py, &Hector::LorentzVector::setPy )
    .add_property( "pz", &Hector::LorentzVector::pz, &Hector::LorentzVector::setPz )
    .add_property( "e", &Hector::LorentzVector::e, &Hector::LorentzVector::setE )
    .def( "vect", &Hector::LorentzVector::vect )
  ;

  Hector::Matrix ( Hector::Matrix::*inverse_except )() const = &Hector::Matrix::inverse;
  //double& ( Hector::Matrix::*mat_elem )( int i, int j ) = &Hector::Matrix::operator();
  py::class_<Hector::Matrix>( "Matrix", "A generic matrix (often used for propagation)" )
    .def( py::self_ns::str( py::self_ns::self ) )
    .def( py::self += py::other<Hector::Matrix>() ).def( py::self -= py::other<Hector::Matrix>() )
    .def( "inverse", inverse_except, "The inversed matrix (when possible)" )
    .def( "transpose", &Hector::Matrix::T, "The transposed matrix" )
    //.def( "__getitem__", mat_elem, py::return_value_policy<py::reference_existing_object>() )
    .add_property( "trace", &Hector::Matrix::trace, "Matrix trace" )
    .add_property( "determinant", &Hector::Matrix::determinant, "Matrix determinant" )
  ;

  //----- EXCEPTIONS

  py::enum_<Hector::ExceptionType>( "ExceptionType" )
    .value( "undefined", Hector::ExceptionType::Undefined )
    .value( "info", Hector::ExceptionType::Info )
    .value( "justWarning", Hector::ExceptionType::JustWarning )
    .value( "fatal", Hector::ExceptionType::Fatal )
  ;

  py::class_<Hector::Exception> except( "Exception", py::init<const char*,const char*,py::optional<Hector::ExceptionType,int> >() ); except
    .add_property( "type", &Hector::Exception::type )
    .add_property( "message", &Hector::Exception::what )
    .add_property( "errorNumber", &Hector::Exception::errorNumber )
    .add_property( "from", &Hector::Exception::from )
  ;
  except_type = except.ptr();
  py::register_exception_translator<Hector::Exception>( &translate_exception );
  py::class_<Hector::ParticleStoppedException, py::bases<Hector::Exception> > psexcept( "ParticleStoppedException", py::init<const char*,const Hector::Element::ElementBase*,py::optional<Hector::ExceptionType,const char*> >() ); psexcept
    .add_property( "stoppingElement", py::make_function( &Hector::ParticleStoppedException::stoppingElement, py::return_value_policy<py::reference_existing_object>() ) )
  ;
  ps_except_type = psexcept.ptr();
  py::register_exception_translator<Hector::ParticleStoppedException>( &translate_ps_exception );

  //----- RUN PARAMETERS

  py::class_<Hector::Parameters, std::shared_ptr<Hector::Parameters>, boost::noncopyable>( "Parameters", py::init<>() )
    .def( "get", &Hector::Parameters::get ).staticmethod( "get" )
    .add_property( "beamEnergy", &Hector::Parameters::beamEnergy, &Hector::Parameters::setBeamEnergy, "Default beam energy (in GeV)" )
    .add_property( "beamParticlesMass", &Hector::Parameters::beamParticlesMass, &Hector::Parameters::setBeamParticlesMass, "Default beam particles mass (in GeV/c2)" )
    .add_property( "beamParticlesCharge", &Hector::Parameters::beamParticlesCharge, &Hector::Parameters::setBeamParticlesCharge, "Default beam particles charge (in e)" )
    .add_property( "loggingThreshold", &Hector::Parameters::loggingThreshold, &Hector::Parameters::setLoggingThreshold, "Hector verbosity" )
    .add_property( "useRelativeEnergy", &Hector::Parameters::useRelativeEnergy, &Hector::Parameters::setUseRelativeEnergy )
    .add_property( "correctBeamlineOverlaps", &Hector::Parameters::correctBeamlineOverlaps, &Hector::Parameters::setCorrectBeamlineOverlaps )
    .add_property( "computeApertureAcceptance", &Hector::Parameters::computeApertureAcceptance, &Hector::Parameters::setComputeApertureAcceptance )
    .add_property( "enableKickers", &Hector::Parameters::enableKickers, &Hector::Parameters::setEnableKickers )
    .add_property( "enableDipoles", &Hector::Parameters::enableDipoles, &Hector::Parameters::setEnableDipoles )
  ;

  //----- BEAM PROPERTIES

  py::class_<Hector::StateVector>( "StateVector" )
    .def( py::init<Hector::Vector,double>() )
    .def( py::init<Hector::LorentzVector,Hector::TwoVector>() )
    .def( py::init<Hector::TwoVector,Hector::TwoVector,double,double>() )
    //.def( py::self += py::other<Hector::StateVector>() )
    .def( py::self_ns::str( py::self_ns::self ) )
    .add_property( "energy", &Hector::StateVector::energy, &Hector::StateVector::setEnergy, "Particle energy (in GeV)" )
    .add_property( "xi", &Hector::StateVector::xi, &Hector::StateVector::setXi, "Particle momentum loss" )
    .add_property( "momentum", &Hector::StateVector::momentum, &Hector::StateVector::setMomentum, "Particle 4-momentum" )
    .add_property( "kick", &Hector::StateVector::kick, &Hector::StateVector::setKick, "Kick" )
    .add_property( "x", &Hector::StateVector::x, &Hector::StateVector::setX, "Particle horizontal coordinate (in metres)" )
    .add_property( "Tx", &Hector::StateVector::Tx, &Hector::StateVector::setTx, "Particle horizontal scattering angle (in radians)" )
    .add_property( "y", &Hector::StateVector::y, &Hector::StateVector::setY, "Particle vertical coordinate (in metres)" )
    .add_property( "Ty", &Hector::StateVector::Ty, &Hector::StateVector::setTy, "Particle vertical scattering angle (in radians)" )
  ;

  void ( Hector::Particle::*addPosition_vec )( double, const Hector::StateVector&, bool ) = &Hector::Particle::addPosition;
  void ( Hector::Particle::*addPosition_pos )( const Hector::Particle::Position&, bool ) = &Hector::Particle::addPosition;
  py::class_<Hector::Particle>( "Particle" )
    .def( py::init<Hector::LorentzVector,int>() )
    .def( py::init<Hector::StateVector,double>() )
    .def( "__str__", &dump_particle )
    .add_property( "pdgId", &Hector::Particle::pdgId, &Hector::Particle::setPDGid )
    .add_property( "charge", &Hector::Particle::charge, &Hector::Particle::setCharge )
    .def( "clear", &Hector::Particle::clear )
    .def( "momentumAt", &Hector::Particle::momentumAt )
    .def( "stateVectorAt", &Hector::Particle::stateVectorAt )
    .add_property( "positions", particle_positions )
    .def( "addPosition", addPosition_pos, particle_add_position_pos_overloads() )
    .def( "addPosition", addPosition_vec, particle_add_position_vec_overloads() )
  ;

  //----- BEAM PRODUCERS

  //py::class_<BeamProducerWrap>( "GaussianParticleGun", py::init<py::optional<float,float,float,float,float,float,float,float,float,float,float,float> >() )
  py::class_<BeamProducerWrap<Hector::BeamProducer::gaussianParticleGun> >( "GaussianParticleGun" )
    .def( py::init<py::dict&>() )
    .def( "shoot", &Hector::BeamProducer::gaussianParticleGun::shoot, "Shoot a single particle" )
    .add_property( "mass", &Hector::BeamProducer::gaussianParticleGun::particleMass, &Hector::BeamProducer::gaussianParticleGun::setParticleMass, "Idividual particles mass (in GeV/c2)" )
    .add_property( "charge", &Hector::BeamProducer::gaussianParticleGun::particleCharge, &Hector::BeamProducer::gaussianParticleGun::setParticleCharge, "Individual particles charge (in e)" )
    .def( "smearTx", &Hector::BeamProducer::gaussianParticleGun::setTXparams, "Smear the beam particles horizontal scattering angle (in rad)" )
    .def( "smearTy", &Hector::BeamProducer::gaussianParticleGun::setTYparams, "Smear the beam particles vertical scattering angle (in rad)" )
    .def( "smearX", &Hector::BeamProducer::gaussianParticleGun::setXparams, "Smear the beam particles horizontal position (in metres)" )
    .def( "smearY", &Hector::BeamProducer::gaussianParticleGun::setYparams, "Smear the beam particles vertical position (in metres)" )
  ;

  //----- BEAMLINE ELEMENTS DEFINITION

  py::enum_<Hector::Element::Type>( "ElementType" )
    .value( "invalid", Hector::Element::Type::anInvalidElement )
    .value( "marker", Hector::Element::Type::aMarker )
    .value( "drift", Hector::Element::Type::aDrift )
    .value( "monitor", Hector::Element::Type::aMonitor )
    .value( "rectangularDipole", Hector::Element::Type::aRectangularDipole )
    .value( "sectorDipole", Hector::Element::Type::aSectorDipole )
    .value( "genericQuadrupole", Hector::Element::Type::aGenericQuadrupole )
    .value( "verticalQuadrupole", Hector::Element::Type::aVerticalQuadrupole )
    .value( "horizontalQuadrupole", Hector::Element::Type::anHorizontalQuadrupole )
    .value( "sextupole", Hector::Element::Type::aSextupole )
    .value( "multipole", Hector::Element::Type::aMultipole )
    .value( "verticalKicker", Hector::Element::Type::aVerticalKicker )
    .value( "horizontalKicker", Hector::Element::Type::anHorizontalKicker )
    .value( "rectangularCollimator", Hector::Element::Type::aRectangularCollimator )
    .value( "ellipticalCollimator", Hector::Element::Type::anEllipticalCollimator )
    .value( "circularCollimator", Hector::Element::Type::aCircularCollimator )
    .value( "placeholder", Hector::Element::Type::aPlaceholder )
    .value( "instrument", Hector::Element::Type::anInstrument )
    .value( "solenoid", Hector::Element::Type::aSolenoid )
  ;

  void ( Hector::Element::ElementBase::*set_aperture_ptr )( Hector::Aperture::ApertureBase* ) = &Hector::Element::ElementBase::setAperture;
  py::class_<ElementBaseWrap, std::shared_ptr<Hector::Element::ElementBase>, boost::noncopyable>( "Element", "A base beamline element object", py::no_init )
    .def( "matrix", py::pure_virtual( &Hector::Element::ElementBase::matrix ) )
    .def( "clone", py::pure_virtual( &Hector::Element::ElementBase::clone ), py::return_value_policy<py::return_by_value>() )
    .add_property( "aperture", py::make_function( &Hector::Element::ElementBase::aperture, py::return_value_policy<py::return_by_value>() ), set_aperture_ptr )
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
  py::register_ptr_to_python<Hector::Element::ElementBase*>();

  //--- passive elements
  convertElement<Hector::Element::Drift, py::init<std::string,float,float> >( "Drift" );
  convertElement<Hector::Element::Marker, py::init<std::string,float,float> >( "Marker" );

  //--- dipoles
  convertElement<Hector::Element::SectorDipole, py::init<std::string,float,float,float> >( "SectorDipole" );
  convertElement<Hector::Element::RectangularDipole, py::init<std::string,float,float,float> >( "RectangularDipole" );

  //--- quadrupoles
  convertElement<Hector::Element::HorizontalQuadrupole, py::init<std::string,float,float,float> >( "HorizontalQuadrupole" );
  convertElement<Hector::Element::VerticalQuadrupole, py::init<std::string,float,float,float> >( "VerticalQuadrupole" );

  //--- kickers
  convertElement<Hector::Element::HorizontalKicker, py::init<std::string,float,float,float> >( "HorizontalKicker" );
  convertElement<Hector::Element::VerticalKicker, py::init<std::string,float,float,float> >( "VerticalKicker" );

  //----- APERTURES DEFINITION

  py::class_<ApertureBaseWrap, std::shared_ptr<Hector::Aperture::ApertureBase>, boost::noncopyable>( "Aperture", "A base aperture object", py::no_init )
    .def( "clone", py::pure_virtual( &Hector::Aperture::ApertureBase::clone ), py::return_value_policy<py::return_by_value>(), "Deep copy of this aperture" )
    .add_property( "type", &Hector::Aperture::ApertureBase::type, &Hector::Aperture::ApertureBase::setType, "Geometrical type" )
    .add_property( "position", &Hector::Aperture::ApertureBase::position, &Hector::Aperture::ApertureBase::setPosition, "Barycentre position (coordinates in metres)" )
    .add_property( "x", &Hector::Aperture::ApertureBase::x, &Hector::Aperture::ApertureBase::setX, "Horizontal barycentre coordinate (in metres)" )
    .add_property( "y", &Hector::Aperture::ApertureBase::y, &Hector::Aperture::ApertureBase::setY, "Vertical barycentre coordinate (in metres)" )
    .def( "offset", &Hector::Aperture::ApertureBase::offset, "Offset the barycentre by a given vertical-horizontal coordinate" )
  ;
  py::register_ptr_to_python<Hector::Aperture::ApertureBase*>();

  convertAperture<Hector::Aperture::CircularAperture, py::init<float,Hector::TwoVector> >( "CircularAperture" );
  convertAperture<Hector::Aperture::EllipticAperture, py::init<float,float,Hector::TwoVector> >( "EllipticAperture" );
  convertAperture<Hector::Aperture::RectangularAperture, py::init<float,float,Hector::TwoVector> >( "RectangularAperture" );
  convertAperture<Hector::Aperture::RectEllipticAperture, py::init<float,float,float,float,Hector::TwoVector> >( "RectEllipticAperture" );

  //----- BEAMLINE DEFINITION

  std::shared_ptr<Hector::Element::ElementBase>& ( Hector::Beamline::*get_by_name )( std::string ) = &Hector::Beamline::getElement;
  std::shared_ptr<Hector::Element::ElementBase>& ( Hector::Beamline::*get_by_spos )( float ) = &Hector::Beamline::getElement;
  py::class_<Hector::Beamline>( "Beamline", "A collection of elements composing a beamline" )
    .def( "__str__", &dump_beamline )
    .def( "dump", &Hector::Beamline::dump, beamline_dump_overloads() )
    .add_property( "length", &Hector::Beamline::length, &Hector::Beamline::setLength, "Total beamline length (in metres)" )
    .add_property( "elements", beamline_elements, "Collection of beamline elements" )
    .def( "matrix", &Hector::Beamline::matrix, "Get the propagation matrix for the full beamline", py::args( "energy loss", "particle mass", "particle charge" ) )
    .def( "sequencedBeamline", &Hector::Beamline::sequencedBeamline, "Get the sequenced (spaces as drifts, propagation-safe) version of the beamline" ).staticmethod( "sequencedBeamline" )
    .def( "clear", &Hector::Beamline::clear, "Clear the beamline from all its elements" )
    .def( "addElement", &Hector::Beamline::addElement, "Add a single element into the beamline collection", py::args( "element to add" ) )
    .def( "getElement", get_by_name, py::return_value_policy<py::return_by_value>(), "Get a beamline element by its name", py::args( "element name" ) )
    .def( "getElement", get_by_spos, py::return_value_policy<py::return_by_value>(), "Get a beamline element by its s-position", py::args( "element s-position" ) )
  ;

  //----- PROPAGATOR

  void ( Hector::Propagator::*propagate_single )( Hector::Particle&, float ) const = &Hector::Propagator::propagate;
  void ( Hector::Propagator::*propagate_multi )( Hector::Particles&, float ) const = &Hector::Propagator::propagate;
  py::class_<Hector::Propagator>( "Propagator", "Beamline propagation helper class", py::init<const Hector::Beamline*>() )
    .def( "propagate", propagate_single, "Propagate a single particle into the beamline", py::args( "particle object", "maximal s-position for the propagation" ) )
    .def( "propagate", propagate_multi, "Propagate a collection of particles into the beamline", py::args( "particles object collection", "maximal s-position for the propagation" ) )
  ;

  //----- I/O HANDLERS

  py::class_<Hector::IO::MADX>( "MadXparser", "A MadX Twiss files parser", py::init<const char*,const char*,int,py::optional<float> >() )
    .add_property( "beamline", py::make_function( &Hector::IO::MADX::beamline, py::return_value_policy<py::reference_existing_object>() ), "Beamline object parsed from the MadX Twiss file" )
    .add_property( "romanPots", &Hector::IO::MADX::romanPots, "List of Roman pots along the beamline" )
  ;
}
