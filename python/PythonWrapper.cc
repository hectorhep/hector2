#include "Hector/Exception.h"
#include "Hector/ParticleStoppedException.h"
#include "Hector/Parameters.h"

#include "Hector/Propagator.h"
#include "Hector/Particle.h"
#include "Hector/StateVector.h"

#include "Hector/Beamline.h"

#include "Hector/Elements/ElementBase.h"
#include "Hector/Elements/Drift.h"
#include "Hector/Elements/Dipole.h"
#include "Hector/Elements/Quadrupole.h"
#include "Hector/Elements/Kicker.h"
#include "Hector/Elements/Collimator.h"

#include "Hector/Apertures/ApertureBase.h"
#include "Hector/Apertures/Circular.h"
#include "Hector/Apertures/Elliptic.h"
#include "Hector/Apertures/Rectangular.h"
#include "Hector/Apertures/RectElliptic.h"

#include "Hector/IO/TwissHandler.h"
#include "Hector/IO/HBLFileHandler.h"

#include "Hector/Utils/BeamProducer.h"
#include <CLHEP/Matrix/Matrix.h>
#include <CLHEP/Random/RandGauss.h>

#include <map>
#include <memory>
#include <sstream>
#include <time.h>

#include <boost/version.hpp>
#include <boost/config.hpp>

//----- FIRST START WITH SOME UTILITIES

#if BOOST_VERSION < 105300 || defined BOOST_NO_CXX11_SMART_PTR

// https://github.com/mapnik/mapnik/issues/2022
namespace boost {
  template <class T>
  const T* get_pointer(const std::shared_ptr<T>& p) {
    return p.get();
  }
  template <class T>
  T* get_pointer(std::shared_ptr<T>& p) {
    return p.get();
  }
}  // namespace boost
#endif

#include <boost/python.hpp>
#include <datetime.h>

//----- SOME OVERLOADED FUNCTIONS/METHODS HELPERS

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(particle_add_position_pos_overloads, addPosition, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(particle_add_position_vec_overloads, addPosition, 2, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(beamline_dump_overloads, dump, 0, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(beamline_matrix, matrix, 1, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(element_matrix, matrix, 1, 3)

namespace {
  namespace py = boost::python;

  std::string dump_particle(const hector::Particle& part) {
    std::ostringstream os;
    part.dump(os);
    return os.str();
  }
  std::string dump_beamline(const hector::Beamline& bl) {
    std::ostringstream os;
    bl.dump(os, true);
    return os.str();
  }
  hector::Matrix invert_matrix(const hector::Matrix& mat) {
    int err;
    hector::Matrix out = mat.inverse(err);
    if (err != 0)
      throw hector::Exception(__PRETTY_FUNCTION__, hector::JustWarning) << "Failed to invert the matrix";
    return out;
  }
  //--- helper python <-> C++ converters
  template <class T, class U>
  py::dict to_python_dict(std::map<T, U>& map) {
    py::dict dictionary;
    for (auto& it : map)
      dictionary[it.first] = it.second;
    return dictionary;
  }
  template <class T, class U>
  py::dict to_python_dict_c(std::map<T, U> map) {
    py::dict dictionary;
    for (auto& it : map)
      dictionary[it.first] = it.second;
    return dictionary;
  }
  template <class T>
  py::list to_python_list(std::vector<T>& vec) {
    py::list list;
    for (auto& it : vec)
      list.append(it);
    return list;
  }
  template <class T>
  py::list to_python_list_c(std::vector<T> vec) {
    py::list list;
    for (auto& it : vec)
      list.append(it);
    return list;
  }
  py::dict particle_positions(hector::Particle& part) {
    return to_python_dict<double, hector::StateVector>(part.positions());
  }
  py::list beamline_elements(hector::Beamline& bl) {
    return to_python_list<std::shared_ptr<hector::element::ElementBase> >(bl.elements());
  }
  py::list beamline_found_elements(hector::Beamline& bl, const char* regex) {
    return to_python_list_c<std::shared_ptr<hector::element::ElementBase> >(bl.find(regex));
  }

  py::dict twiss_parser_header(hector::io::Twiss& parser) {
    py::dict out = to_python_dict_c<std::string, std::string>(parser.headerStrings());
    out.update(to_python_dict_c<std::string, float>(parser.headerFloats()));
    if (out.has_key("timestamp")) {
      time_t dt = static_cast<long>(py::extract<float>(out.get("timestamp")));
      std::tm tm;
      localtime_r(&dt, &tm);
      PyDateTime_IMPORT;
      out["production_date"] = py::handle<>(PyDateTime_FromDateAndTime(
          tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, 0.));
    }
    return out;
  }

  PyObject *except_type = nullptr, *ps_except_type = nullptr;

  void translate_exception(const hector::Exception& e) {
    if (except_type == NULL)
      return;
    PyErr_SetObject(except_type, py::object(e).ptr());
  }
  void translate_ps_exception(const hector::ParticleStoppedException& e) {
    if (ps_except_type == NULL)
      return;
    PyErr_SetObject(ps_except_type, py::object(e).ptr());
  }

  //--- helper beamline elements definitions
  struct ElementBaseWrap : hector::element::ElementBase, py::wrapper<hector::element::ElementBase> {
    ElementBaseWrap() : hector::element::ElementBase(hector::element::anInvalidElement) {}
    std::shared_ptr<hector::element::ElementBase> clone() const override {
      if (py::override n = this->get_override("clone"))
        return n();
      return hector::element::ElementBase::clone();
    }
    hector::Matrix matrix(double eloss, double mp, int qp) const override {
      if (py::override m = this->get_override("matrix"))
        return m(eloss, mp, qp);
      return hector::element::ElementBase::matrix(eloss, mp, qp);
    }
  };
  template <class T, class init = py::init<std::string, double, double, double> >
  void convertElement(const char* name) {
    py::class_<T, py::bases<hector::element::ElementBase> >(name, init())
        .def("clone", &T::clone, py::return_value_policy<py::return_by_value>())
        .def("matrix", &T::matrix, element_matrix());
  }

  struct ApertureBaseWrap : hector::aperture::ApertureBase, py::wrapper<hector::aperture::ApertureBase> {
    ApertureBaseWrap()
        : hector::aperture::ApertureBase(
              hector::aperture::anInvalidAperture, hector::TwoVector(), std::vector<double>{}) {}
    std::shared_ptr<hector::aperture::ApertureBase> clone() const override { return this->get_override("clone")(); }
  };
  template <class T, class init>
  void convertAperture(const char* name) {
    py::class_<T, py::bases<hector::aperture::ApertureBase> >(name, init())
        .def("clone", &T::clone, py::return_value_policy<py::return_by_value>());
  }
}  // namespace

//----- AND HERE COMES THE MODULE

BOOST_PYTHON_MODULE(pyhector) {
  //----- GENERAL HELPERS

  py::class_<hector::TwoVector>("TwoVector", "A generic 2-vector for planar coordinates")
      .def(py::init<double, double>())
      .def(py::self_ns::str(py::self_ns::self))
      .def(py::self += py::other<hector::TwoVector>())
      .def(py::self -= py::other<hector::TwoVector>())
      .add_property("x", &hector::TwoVector::x, &hector::TwoVector::setX, "Horizontal coordinate")
      .add_property("y", &hector::TwoVector::y, &hector::TwoVector::setY, "Vertical coordinate")
      .add_property("mag", &hector::TwoVector::mag, &hector::TwoVector::setMag, "2-vector norm")
      .add_property("phi", &hector::TwoVector::phi, &hector::TwoVector::setPhi, "2-vector angle");

  double (hector::ThreeVector::*theta_val)() const = &hector::ThreeVector::theta;
  py::class_<hector::ThreeVector>("ThreeVector", "A generic 3-vector for spatial coordinates")
      .def(py::init<double, double, double>())
      .def(py::self_ns::str(py::self_ns::self))
      .def(py::self += py::other<hector::ThreeVector>())
      .def(py::self -= py::other<hector::ThreeVector>())
      .add_property("x", &hector::ThreeVector::x, &hector::ThreeVector::setX, "Horizontal coordinate")
      .add_property("y", &hector::ThreeVector::y, &hector::ThreeVector::setY, "Vertical coordinate")
      .add_property("z", &hector::ThreeVector::z, &hector::ThreeVector::setZ, "Longitudinal coordinate")
      .add_property("mag", &hector::ThreeVector::mag, &hector::ThreeVector::setMag, "3-vector norm")
      .add_property("theta", theta_val, &hector::ThreeVector::setTheta, "3-vector polar angle")
      .add_property("phi", &hector::ThreeVector::phi, &hector::ThreeVector::setPhi, "3-vector azimuthal angle")
      .def("deltaR", &hector::ThreeVector::deltaR);

  py::class_<hector::LorentzVector>("LorentzVector")
      .def(py::init<double, double, double, double>())
      .def(py::self_ns::str(py::self_ns::self))
      .def(py::self += py::other<hector::LorentzVector>())
      .def(py::self -= py::other<hector::LorentzVector>())
      .add_property("x", &hector::LorentzVector::x, &hector::LorentzVector::setX, "Horizontal coordinate")
      .add_property("y", &hector::LorentzVector::y, &hector::LorentzVector::setY, "Vertical coordinate")
      .add_property("z", &hector::LorentzVector::z, &hector::LorentzVector::setZ, "Longitudinal coordinate")
      .add_property("t", &hector::LorentzVector::t, &hector::LorentzVector::setT, "Time coordinate")
      .add_property("px", &hector::LorentzVector::px, &hector::LorentzVector::setPx, "Horizontal momentum")
      .add_property("py", &hector::LorentzVector::py, &hector::LorentzVector::setPy, "Vertical momentum")
      .add_property("pz", &hector::LorentzVector::pz, &hector::LorentzVector::setPz, "Longitudinal momentum")
      .add_property("e", &hector::LorentzVector::e, &hector::LorentzVector::setE, "Energy")
      .def("vect", &hector::LorentzVector::vect);

  //double& ( hector::Matrix::*mat_elem )( int i, int j ) = &hector::Matrix::operator();
  py::class_<hector::Matrix>("Matrix", "A generic matrix (often used for propagation)")
      .def(py::self_ns::str(py::self_ns::self))
      .def(py::self += py::other<hector::Matrix>())
      .def(py::self -= py::other<hector::Matrix>())
      .def(py::self *= py::other<double>())
      .def(py::self * py::other<hector::Matrix>())
      .def("transpose", &hector::Matrix::T, "The transposed matrix")
      //.def( "__getitem__", mat_elem, py::return_value_policy<py::reference_existing_object>() )
      .add_property("inverse", &invert_matrix, "The inversed matrix (when possible)")
      .add_property("trace", &hector::Matrix::trace, "Matrix trace")
      .add_property("determinant", &hector::Matrix::determinant, "Matrix determinant");

  //----- EXCEPTIONS

  py::enum_<hector::ExceptionType>("ExceptionType")
      .value("undefined", hector::ExceptionType::Undefined)
      .value("debug", hector::ExceptionType::Debug)
      .value("info", hector::ExceptionType::Info)
      .value("justWarning", hector::ExceptionType::JustWarning)
      .value("fatal", hector::ExceptionType::Fatal);

  py::class_<hector::Exception> except("Exception", py::init<const char*, py::optional<hector::ExceptionType, int> >());
  except.add_property("type", &hector::Exception::type)
      .add_property("message", &hector::Exception::what)
      .add_property("errorNumber", &hector::Exception::errorNumber)
      .add_property("from", &hector::Exception::from);
  py::class_<hector::ParticleStoppedException, py::bases<hector::Exception> > psexcept(
      "ParticleStoppedException",
      py::init<const char*, py::optional<hector::ExceptionType, const hector::element::ElementBase*> >());
  psexcept.add_property("stoppingElement",
                        py::make_function(&hector::ParticleStoppedException::stoppingElement,
                                          py::return_value_policy<py::reference_existing_object>()));

  // register the exceptions
  except_type = except.ptr();
  ps_except_type = psexcept.ptr();
  py::register_exception_translator<hector::Exception>(&translate_exception);
  py::register_exception_translator<hector::ParticleStoppedException>(&translate_ps_exception);

  //----- RUN PARAMETERS

  py::class_<hector::Parameters, std::shared_ptr<hector::Parameters>, boost::noncopyable>("Parameters", py::init<>())
      .def("get", &hector::Parameters::get)
      .staticmethod("get")
      .add_property("beamEnergy",
                    &hector::Parameters::beamEnergy,
                    &hector::Parameters::setBeamEnergy,
                    "Default beam energy (in GeV)")
      .add_property("beamParticlesMass",
                    &hector::Parameters::beamParticlesMass,
                    &hector::Parameters::setBeamParticlesMass,
                    "Default beam particles mass (in GeV/c2)")
      .add_property("beamParticlesCharge",
                    &hector::Parameters::beamParticlesCharge,
                    &hector::Parameters::setBeamParticlesCharge,
                    "Default beam particles charge (in e)")
      .add_property("loggingThreshold",
                    &hector::Parameters::loggingThreshold,
                    &hector::Parameters::setLoggingThreshold,
                    "hector verbosity")
      .add_property(
          "useRelativeEnergy", &hector::Parameters::useRelativeEnergy, &hector::Parameters::setUseRelativeEnergy)
      .add_property("correctBeamlineOverlaps",
                    &hector::Parameters::correctBeamlineOverlaps,
                    &hector::Parameters::setCorrectBeamlineOverlaps)
      .add_property("computeApertureAcceptance",
                    &hector::Parameters::computeApertureAcceptance,
                    &hector::Parameters::setComputeApertureAcceptance)
      .add_property("enableKickers", &hector::Parameters::enableKickers, &hector::Parameters::setEnableKickers)
      .add_property("enableDipoles", &hector::Parameters::enableDipoles, &hector::Parameters::setEnableDipoles);

  //----- BEAM PROPERTIES

  py::class_<hector::StateVector>("StateVector")
      .def(py::init<hector::Vector, double>())
      .def(py::init<hector::LorentzVector, hector::TwoVector>())
      .def(py::init<hector::TwoVector, hector::TwoVector, double, double>())
      //.def( py::self += py::other<hector::StateVector>() )
      .def(py::self_ns::str(py::self_ns::self))
      .add_property("energy", &hector::StateVector::energy, &hector::StateVector::setEnergy, "Particle energy (in GeV)")
      .add_property("xi", &hector::StateVector::xi, &hector::StateVector::setXi, "Particle momentum loss")
      .add_property(
          "momentum", &hector::StateVector::momentum, &hector::StateVector::setMomentum, "Particle 4-momentum")
      .add_property("kick", &hector::StateVector::kick, &hector::StateVector::setKick, "Kick")
      .add_property(
          "x", &hector::StateVector::x, &hector::StateVector::setX, "Particle horizontal coordinate (in metres)")
      .add_property("Tx",
                    &hector::StateVector::Tx,
                    &hector::StateVector::setTx,
                    "Particle horizontal scattering angle (in radians)")
      .add_property(
          "y", &hector::StateVector::y, &hector::StateVector::setY, "Particle vertical coordinate (in metres)")
      .add_property("Ty",
                    &hector::StateVector::Ty,
                    &hector::StateVector::setTy,
                    "Particle vertical scattering angle (in radians)");

  void (hector::Particle::*addPosition_vec)(double, const hector::StateVector&, bool) = &hector::Particle::addPosition;
  void (hector::Particle::*addPosition_pos)(const hector::Particle::Position&, bool) = &hector::Particle::addPosition;
  py::class_<hector::Particle>("Particle")
      .def(py::init<hector::LorentzVector, int>())
      .def(py::init<hector::StateVector, double>())
      .def("__str__", &dump_particle)
      .add_property("pdgId", &hector::Particle::pdgId, &hector::Particle::setPDGid)
      .add_property("charge", &hector::Particle::charge, &hector::Particle::setCharge)
      .def("clear", &hector::Particle::clear)
      .def("momentumAt", &hector::Particle::momentumAt)
      .def("stateVectorAt", &hector::Particle::stateVectorAt)
      .add_property("positions", particle_positions)
      .def("addPosition", addPosition_pos, particle_add_position_pos_overloads())
      .def("addPosition", addPosition_vec, particle_add_position_vec_overloads());

  //----- BEAM PRODUCERS

  py::class_<hector::beam::GaussianParticleGun>("GaussianParticleGun")
      .def("shoot", &hector::beam::GaussianParticleGun::shoot, "Shoot a single particle")
      .add_property("mass",
                    &hector::beam::GaussianParticleGun::particleMass,
                    &hector::beam::GaussianParticleGun::setParticleMass,
                    "Individual particles mass (in GeV/c2)")
      .add_property("charge",
                    &hector::beam::GaussianParticleGun::particleCharge,
                    &hector::beam::GaussianParticleGun::setParticleCharge,
                    "Individual particles charge (in e)")
      .def("smearEnergy", &hector::beam::GaussianParticleGun::smearEnergy, "Particles energy smearing (in GeV)")
      .def("smearTx",
           &hector::beam::GaussianParticleGun::smearTx,
           "Smear the beam particles horizontal scattering angle (in rad)")
      .def("smearTy",
           &hector::beam::GaussianParticleGun::smearTy,
           "Smear the beam particles vertical scattering angle (in rad)")
      .def("smearX",
           &hector::beam::GaussianParticleGun::smearX,
           "Smear the beam particles horizontal position (in metres)")
      .def("smearY",
           &hector::beam::GaussianParticleGun::smearY,
           "Smear the beam particles vertical position (in metres)");

  //----- BEAMLINE ELEMENTS DEFINITION

  py::enum_<hector::element::Type>("ElementType")
      .value("invalid", hector::element::Type::anInvalidElement)
      .value("marker", hector::element::Type::aMarker)
      .value("drift", hector::element::Type::aDrift)
      .value("monitor", hector::element::Type::aMonitor)
      .value("rectangularDipole", hector::element::Type::aRectangularDipole)
      .value("sectorDipole", hector::element::Type::aSectorDipole)
      .value("genericQuadrupole", hector::element::Type::aGenericQuadrupole)
      .value("verticalQuadrupole", hector::element::Type::aVerticalQuadrupole)
      .value("horizontalQuadrupole", hector::element::Type::anHorizontalQuadrupole)
      .value("sextupole", hector::element::Type::aSextupole)
      .value("multipole", hector::element::Type::aMultipole)
      .value("verticalKicker", hector::element::Type::aVerticalKicker)
      .value("horizontalKicker", hector::element::Type::anHorizontalKicker)
      .value("rectangularCollimator", hector::element::Type::aRectangularCollimator)
      .value("ellipticalCollimator", hector::element::Type::anEllipticalCollimator)
      .value("circularCollimator", hector::element::Type::aCircularCollimator)
      .value("collimator", hector::element::Type::aCollimator)
      .value("placeholder", hector::element::Type::aPlaceholder)
      .value("instrument", hector::element::Type::anInstrument)
      .value("solenoid", hector::element::Type::aSolenoid);

  void (hector::element::ElementBase::*set_aperture_ptr)(hector::aperture::ApertureBase*) =
      &hector::element::ElementBase::setAperture;
  py::class_<ElementBaseWrap, std::shared_ptr<hector::element::ElementBase>, boost::noncopyable>(
      "Element", "A base beamline element object", py::no_init)
      .def("matrix",
           py::pure_virtual(&hector::element::ElementBase::matrix),
           (py::arg("energy loss"),
            py::arg("particle mass (in GeV/c2)") = hector::Parameters::get()->beamParticlesMass(),
            py::arg("particle charge (in e)") = hector::Parameters::get()->beamParticlesCharge()))
      .def("clone",
           py::pure_virtual(&hector::element::ElementBase::clone),
           py::return_value_policy<py::return_by_value>())
      .add_property(
          "aperture",
          py::make_function(&hector::element::ElementBase::aperture, py::return_value_policy<py::return_by_value>()),
          set_aperture_ptr)
      .add_property("name", &hector::element::ElementBase::name, &hector::element::ElementBase::setName)
      .add_property("type", &hector::element::ElementBase::type, &hector::element::ElementBase::setType)
      .add_property("s", &hector::element::ElementBase::s, &hector::element::ElementBase::setS)
      .add_property("length", &hector::element::ElementBase::length, &hector::element::ElementBase::setLength)
      .add_property("position", &hector::element::ElementBase::position, &hector::element::ElementBase::setPosition)
      .add_property("angles", &hector::element::ElementBase::angles, &hector::element::ElementBase::setAngles)
      .add_property("magneticStrength",
                    &hector::element::ElementBase::magneticStrength,
                    &hector::element::ElementBase::setMagneticStrength)
      .add_property("beta", &hector::element::ElementBase::beta, &hector::element::ElementBase::setBeta)
      .add_property(
          "dispersion", &hector::element::ElementBase::dispersion, &hector::element::ElementBase::setDispersion)
      .add_property("relativePosition",
                    &hector::element::ElementBase::relativePosition,
                    &hector::element::ElementBase::setRelativePosition)
      .add_property("parent",
                    py::make_function(&hector::element::ElementBase::parentElement,
                                      py::return_value_policy<py::return_by_value>()),
                    &hector::element::ElementBase::setParentElement)
      .def("offsetS",
           &hector::element::ElementBase::offsetS,
           "Offset the element longitudinal coordinate by a given distance");
  py::register_ptr_to_python<hector::element::ElementBase*>();

  //--- passive elements
  convertElement<hector::element::Drift, py::init<std::string, py::optional<double, double> > >("Drift");
  convertElement<hector::element::Marker, py::init<std::string, double, py::optional<double> > >("Marker");

  //--- dipoles
  convertElement<hector::element::SectorDipole, py::init<std::string, double, double, double> >("SectorDipole");
  convertElement<hector::element::RectangularDipole, py::init<std::string, double, double, double> >(
      "RectangularDipole");

  //--- quadrupoles
  convertElement<hector::element::HorizontalQuadrupole, py::init<std::string, double, double, double> >(
      "HorizontalQuadrupole");
  convertElement<hector::element::VerticalQuadrupole, py::init<std::string, double, double, double> >(
      "VerticalQuadrupole");

  //--- kickers
  convertElement<hector::element::HorizontalKicker, py::init<std::string, double, double, double> >("HorizontalKicker");
  convertElement<hector::element::VerticalKicker, py::init<std::string, double, double, double> >("VerticalKicker");

  convertElement<hector::element::Collimator, py::init<std::string, py::optional<double, double> > >("Collimator");

  //----- APERTURES DEFINITION

  py::class_<ApertureBaseWrap, std::shared_ptr<hector::aperture::ApertureBase>, boost::noncopyable>(
      "Aperture", "A base aperture object", py::no_init)
      .def("clone",
           py::pure_virtual(&hector::aperture::ApertureBase::clone),
           py::return_value_policy<py::return_by_value>(),
           "Deep copy of this aperture")
      .add_property(
          "type", &hector::aperture::ApertureBase::type, &hector::aperture::ApertureBase::setType, "Geometrical type")
      .add_property("position",
                    &hector::aperture::ApertureBase::position,
                    &hector::aperture::ApertureBase::setPosition,
                    "Barycentre position (coordinates in metres)")
      .add_property("x",
                    &hector::aperture::ApertureBase::x,
                    &hector::aperture::ApertureBase::setX,
                    "Horizontal barycentre coordinate (in metres)")
      .add_property("y",
                    &hector::aperture::ApertureBase::y,
                    &hector::aperture::ApertureBase::setY,
                    "Vertical barycentre coordinate (in metres)")
      .def("offset",
           &hector::aperture::ApertureBase::offset,
           "Offset the barycentre by a given vertical-horizontal coordinate");
  py::register_ptr_to_python<hector::aperture::ApertureBase*>();

  convertAperture<hector::aperture::Circular, py::init<double, hector::TwoVector> >("CircularAperture");
  convertAperture<hector::aperture::Elliptic, py::init<double, double, hector::TwoVector> >("EllipticAperture");
  convertAperture<hector::aperture::Rectangular, py::init<double, double, hector::TwoVector> >("RectangularAperture");
  convertAperture<hector::aperture::RectElliptic, py::init<double, double, double, double, hector::TwoVector> >(
      "RectEllipticAperture");

  //----- BEAMLINE DEFINITION

  std::shared_ptr<hector::element::ElementBase>& (hector::Beamline::*get_by_name)(const std::string&) =
      &hector::Beamline::get;
  std::shared_ptr<hector::element::ElementBase>& (hector::Beamline::*get_by_spos)(double) = &hector::Beamline::get;
  py::class_<hector::Beamline>("Beamline", "A collection of elements composing a beamline")
      .def("__str__", &dump_beamline)
      .def("dump", &hector::Beamline::dump, beamline_dump_overloads())
      .add_property(
          "length", &hector::Beamline::length, &hector::Beamline::setLength, "Total beamline length (in metres)")
      .add_property(
          "interactionPoint",
          py::make_function(&hector::Beamline::interactionPoint, py::return_value_policy<py::return_by_value>()),
          &hector::Beamline::setInteractionPoint,
          "Bunch crossing element (place where collisions occur)")
      .add_property("elements", beamline_elements, "Collection of beamline elements")
      .def(
          "matrix",
          &hector::Beamline::matrix,
          beamline_matrix())  //, "Get the propagation matrix for the full beamline", py::args( "energy loss", "particle mass", "particle charge" ) )
      .def("sequencedBeamline",
           &hector::Beamline::sequencedBeamline,
           "Get the sequenced (spaces as drifts, propagation-safe) version of the beamline")
      .staticmethod("sequencedBeamline")
      .def("clear", &hector::Beamline::clear, "Clear the beamline from all its elements")
      .def("add",
           &hector::Beamline::add,
           "Add a single element into the beamline collection",
           py::args("element to add"))
      .def("get",
           get_by_name,
           py::return_value_policy<py::return_by_value>(),
           "Get a beamline element by its name",
           py::args("element name"))
      .def("get",
           get_by_spos,
           py::return_value_policy<py::return_by_value>(),
           "Get a beamline element by its s-position",
           py::args("element s-position"))
      .def("offsetElementsAfter", &hector::Beamline::offsetElementsAfter)
      .def("find", beamline_found_elements);

  //----- PROPAGATOR

  void (hector::Propagator::*propagate_single)(hector::Particle&, double) const = &hector::Propagator::propagate;
  void (hector::Propagator::*propagate_multi)(hector::Particles&, double) const = &hector::Propagator::propagate;
  py::class_<hector::Propagator>("Propagator", "Beamline propagation helper class", py::init<const hector::Beamline*>())
      .def("propagate",
           propagate_single,
           "Propagate a single particle into the beamline",
           py::args("particle object", "maximal s-position for the propagation"))
      .def("propagate",
           propagate_multi,
           "Propagate a collection of particles into the beamline",
           py::args("particles object collection", "maximal s-position for the propagation"));

  //----- I/O HANDLERS

  py::class_<hector::io::Twiss>(
      "Twissparser", "A Twiss files parser", py::init<const char*, const char*, py::optional<double, double> >())
      .add_property(
          "beamline",
          py::make_function(&hector::io::Twiss::beamline, py::return_value_policy<py::reference_existing_object>()),
          "Beamline object parsed from the Twiss file")
      .add_property("header", twiss_parser_header);

  py::class_<hector::io::HBL>("HBLparser", "A HBL files parser", py::init<const char*>())
      .add_property(
          "beamline",
          py::make_function(&hector::io::HBL::beamline, py::return_value_policy<py::reference_existing_object>()),
          "Beamline object parsed from the HBL file")
      .def("write", write)
      .staticmethod("write");
}
