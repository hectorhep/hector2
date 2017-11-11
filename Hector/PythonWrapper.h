#ifndef Hector_PythonWrapper_h
#define Hector_PythonWrapper_h

#include <boost/python.hpp>

#include "Elements/ElementBase.h"

namespace
{
  namespace py = boost::python;

  template<class T>
  void convertElementBase( const char* name )
  {
    struct Wrap : T, py::wrapper<T>
    {
      std::shared_ptr<Hector::Element::ElementBase> clone() const { return this->get_override( "clone" )(); }
      CLHEP::HepMatrix matrix( float eloss, float mp, int qp ) const { return this->get_override( "matrix" )( eloss, mp, qp ); }
    };
    py::class_<Wrap, boost::noncopyable, py::bases<Hector::Element::ElementBase> >( name, py::no_init )
      //.def( "clone", py::pure_virtual( &Hector::Element::ElementBase::clone ), py::return_value_policy<py::manage_new_object>() )
      .def( "clone", py::pure_virtual( &Wrap::clone ), py::return_value_policy<py::manage_new_object>() )
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

  /*struct ElementBaseWrap : Hector::Element::ElementBase, py::wrapper<Hector::Element::ElementBase>
  {
    ElementBaseWrap() : Hector::Element::ElementBase( Hector::Element::anInvalidElement ) {}
    std::shared_ptr<Hector::Element::ElementBase> clone() const override { return this->get_override( "clone" )(); }
    std::shared_ptr<ElementBaseWrap> cloneDef() const { return std::make_shared<ElementBaseWrap>( *this ); }
    CLHEP::HepMatrix matrix( float eloss, float mp, int qp ) const override { return this->get_override( "matrix" )( eloss, mp, qp ); }
  };*/
  struct ElementBaseWrap : Hector::Element::ElementBase
  {
    ElementBaseWrap( PyObject* self ) :
      Hector::Element::ElementBase( Hector::Element::anInvalidElement ),
      self_( self ) {}
    PyObject* self_;
    std::shared_ptr<Hector::Element::ElementBase> clone() const { return py::call_method<std::shared_ptr<Hector::Element::ElementBase> >( self_, "clone" ); }
  };
}

#endif

