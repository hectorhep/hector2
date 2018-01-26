#include "Hector/Core/ExceptionType.h"
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

#include "Hector/IO/MADXHandler.h"

#include "Hector/Utils/BeamProducer.h"
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
  template<class T, class U> py::dict to_python_dict( std::map<T,U>& map ) {
    py::dict dictionary;
    for ( auto& it : map ) dictionary[it.first] = it.second;
    return dictionary;
  }
  template<class T> py::list to_python_list( std::vector<T>& vec ) {
    py::list list;
    for ( auto& it : vec ) list.append( it );
    return list;
  }
  template<class T>
  struct PyMap
  {
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

  void translate_exception( const Hector::Exception& e ) {
    switch ( e.type() ) {
      case Hector::Fatal: case Hector::Undefined: PyErr_SetString( PyExc_RuntimeError, e.what() ); return;
      case Hector::JustWarning: PyErr_SetString( PyExc_RuntimeWarning, e.what() ); return;
      case Hector::Info: PyErr_SetString( PyExc_UserWarning, e.what() ); return;
    }
  }
  /*template <class E, class... Policies, class... Args>
  py::class_<E, Policies...> exception_(Args&&... args) {
    py::class_<E, Policies...> cls(std::forward<Args>(args)...);
    py::register_exception_translator<E>([ptr=cls.ptr()](E const& e){
        PyErr_SetObject(ptr, py::object(e).ptr());
    });
    return cls;
  }*/

  template<class T>
  struct BeamProducerWrap : PyMap<float>, T
  {
    BeamProducerWrap() : PyMap<float>(), T() {}
    BeamProducerWrap( py::dict& dict ) :
      PyMap<float>( dict, { { "Emin", "Emax", "smin", "smax", "xmin", "xmax", "ymin", "ymax", "Txmin", "Txmax", "Tymin", "Tymax", "m", "q" } } ),
      T( map["Emin"], map["Emax"], map["smin"], map["smax"],
                                                 map["xmin"], map["xmax"], map["ymin"], map["ymax"],
                                                 map["Txmin"], map["Txmax"], map["Tymin"], map["Tymax"],
                                                 map["m"], map["q"] ) {}
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
}

//----- THEN SOME OVERLOADED FUNCTIONS/METHODS HELPERS

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( particle_add_position_pos_overloads, addPosition, 1, 2 )
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( particle_add_position_vec_overloads, addPosition, 2, 3 )
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( beamline_dump_overloads, dump, 0, 2 )

//----- AND HERE COMES THE MODULE

BOOST_PYTHON_MODULE( pyhector )
{
  //----- GENERAL HELPERS

  py::class_<Hector::LorentzVector>( "LorentzVector" )
    .def( py::init<double,double,double,double>() )
    .def( py::self_ns::str( py::self_ns::self ) )
    .def( py::self += py::other<Hector::LorentzVector>() )
    .add_property( "x", &Hector::LorentzVector::x, &Hector::LorentzVector::setX )
    .add_property( "y", &Hector::LorentzVector::y, &Hector::LorentzVector::setY )
    .add_property( "z", &Hector::LorentzVector::z, &Hector::LorentzVector::setZ )
    .add_property( "t", &Hector::LorentzVector::t, &Hector::LorentzVector::setT )
    .add_property( "px", &Hector::LorentzVector::px, &Hector::LorentzVector::setPx )
    .add_property( "py", &Hector::LorentzVector::py, &Hector::LorentzVector::setPy )
    .add_property( "pz", &Hector::LorentzVector::pz, &Hector::LorentzVector::setPz )
    .add_property( "e", &Hector::LorentzVector::e, &Hector::LorentzVector::setE )
  ;

  //----- EXCEPTIONS

  py::enum_<Hector::ExceptionType>( "ExceptionType" )
    .value( "undefined", Hector::ExceptionType::Undefined )
    .value( "info", Hector::ExceptionType::Info )
    .value( "justWarning", Hector::ExceptionType::JustWarning )
    .value( "fatal", Hector::ExceptionType::Fatal )
  ;

  py::register_exception_translator<Hector::Exception>( &translate_exception );
  /*exception_<Hector::Exception>( "Exception", py::init<std::string,std::string>() )
    .add_property( "message", &Hector::Exception::what )
    //.add_property("extra_data", &MyCPPException::getExtraData)
  ;*/

  //----- RUN PARAMETERS

  py::class_<Hector::Parameters, std::shared_ptr<Hector::Parameters>, boost::noncopyable>( "Parameters", py::init<>() )
    .def( "get", &Hector::Parameters::get ).staticmethod( "get" )
    .add_property( "beamEnergy", &Hector::Parameters::beamEnergy, &Hector::Parameters::setBeamEnergy )
    .add_property( "beamParticlesMass", &Hector::Parameters::beamParticlesMass, &Hector::Parameters::setBeamParticlesMass )
    .add_property( "beamParticlesCharge", &Hector::Parameters::beamParticlesCharge, &Hector::Parameters::setBeamParticlesCharge )
    .add_property( "loggingThreshold", &Hector::Parameters::loggingThreshold, &Hector::Parameters::setLoggingThreshold )
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
    .add_property( "energy", &Hector::StateVector::energy, &Hector::StateVector::setEnergy )
    .add_property( "xi", &Hector::StateVector::xi, &Hector::StateVector::setXi )
    .add_property( "momentum", &Hector::StateVector::momentum, &Hector::StateVector::setMomentum )
    .add_property( "kick", &Hector::StateVector::kick, &Hector::StateVector::setKick )
    .add_property( "x", &Hector::StateVector::x, &Hector::StateVector::setX )
    .add_property( "Tx", &Hector::StateVector::Tx, &Hector::StateVector::setTx )
    .add_property( "y", &Hector::StateVector::y, &Hector::StateVector::setY )
    .add_property( "Ty", &Hector::StateVector::Ty, &Hector::StateVector::setTy )
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
    .def( "positions", particle_positions )
    .def( "addPosition", addPosition_pos, particle_add_position_pos_overloads() )
    .def( "addPosition", addPosition_vec, particle_add_position_vec_overloads() )
  ;

  //----- BEAM PRODUCERS

  //py::class_<BeamProducerWrap>( "GaussianParticleGun", py::init<py::optional<float,float,float,float,float,float,float,float,float,float,float,float> >() )
  py::class_<BeamProducerWrap<Hector::BeamProducer::gaussianParticleGun> >( "GaussianParticleGun" )
    .def( py::init<py::dict&>() )
    .def( "shoot", &Hector::BeamProducer::gaussianParticleGun::shoot )
    .add_property( "mass", &Hector::BeamProducer::gaussianParticleGun::particleMass, &Hector::BeamProducer::gaussianParticleGun::setParticleMass )
    .add_property( "charge", &Hector::BeamProducer::gaussianParticleGun::particleCharge, &Hector::BeamProducer::gaussianParticleGun::setParticleCharge )
    .def( "smearTx", &Hector::BeamProducer::gaussianParticleGun::setTXparams )
    .def( "smearTy", &Hector::BeamProducer::gaussianParticleGun::setTYparams )
    .def( "smearX", &Hector::BeamProducer::gaussianParticleGun::setXparams )
    .def( "smearY", &Hector::BeamProducer::gaussianParticleGun::setYparams )
  ;

  //----- BEAMLINE DEFINITION

  py::class_<ElementBaseWrap, std::shared_ptr<Hector::Element::ElementBase>, boost::noncopyable>( "Element", py::no_init )
    .def( "matrix", py::pure_virtual( &Hector::Element::ElementBase::matrix ) )
    .def( "clone", py::pure_virtual( &Hector::Element::ElementBase::clone ), py::return_value_policy<py::return_by_value>() )
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
  convertElement<Hector::Element::SectorDipole, py::init<std::string,float,float,float> >( "SectorDipole" );
  convertElement<Hector::Element::RectangularDipole, py::init<std::string,float,float,float> >( "RectangularDipole" );

  //--- quadrupoles
  convertElement<Hector::Element::HorizontalQuadrupole, py::init<std::string,float,float,float> >( "HorizontalQuadrupole" );
  convertElement<Hector::Element::VerticalQuadrupole, py::init<std::string,float,float,float> >( "VerticalQuadrupole" );

  //--- kickers
  convertElement<Hector::Element::HorizontalKicker, py::init<std::string,float,float,float> >( "HorizontalKicker" );
  convertElement<Hector::Element::VerticalKicker, py::init<std::string,float,float,float> >( "VerticalKicker" );

  py::class_<Hector::Beamline>( "Beamline" )
    .def( "__str__", &dump_beamline )
    .def( "dump", &Hector::Beamline::dump, beamline_dump_overloads() )
    .def( "elements", beamline_elements )
    .def( "sequencedBeamline", &Hector::Beamline::sequencedBeamline ).staticmethod( "sequencedBeamline" )
    .def( "clear", &Hector::Beamline::clear )
    .def( "addElement", &Hector::Beamline::addElement )
  ;

  //----- PROPAGATOR

  void ( Hector::Propagator::*propagate_single )( Hector::Particle&, float ) const = &Hector::Propagator::propagate;
  void ( Hector::Propagator::*propagate_multi )( Hector::Particles&, float ) const = &Hector::Propagator::propagate;
  py::class_<Hector::Propagator>( "Propagator", py::init<const Hector::Beamline*>() )
    .def( "propagate", propagate_single )
    .def( "propagate", propagate_multi )
  ;

  //----- I/O HANDLERS

  py::class_<Hector::IO::MADX>( "MadXparser", py::init<const char*,const char*,int,py::optional<float> >() )
    .def( "beamline", &Hector::IO::MADX::beamline, py::return_value_policy<py::reference_existing_object>() )
    .def( "romanPots", &Hector::IO::MADX::romanPots )
  ;
}
