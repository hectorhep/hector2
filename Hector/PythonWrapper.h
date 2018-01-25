#ifndef Hector_PythonWrapper_h
#define Hector_PythonWrapper_h

#include <boost/python.hpp>

#include "Elements/ElementBase.h"

namespace
{
  namespace py = boost::python;

  struct ElementBaseWrap : Hector::Element::ElementBase, py::wrapper<Hector::Element::ElementBase>
  {
    ElementBaseWrap() : Hector::Element::ElementBase( Hector::Element::anInvalidElement ) {}
    std::shared_ptr<Hector::Element::ElementBase> clone() const override { return this->get_override( "clone" )(); }
    Hector::Matrix matrix( float eloss, float mp, int qp ) const override { return this->get_override( "matrix" )( eloss, mp, qp ); }
  };

  template<class T>
  void convertElementBase( const char* name )
  {
    struct Wrap : T, py::wrapper<T>
    {
      using T::T;
      std::shared_ptr<Hector::Element::ElementBase> clone() const override { return this->get_override( "clone" )(); }
      Hector::Matrix matrix( float eloss, float mp, int qp ) const override { return this->get_override( "matrix" )( eloss, mp, qp ); }
    };
    py::class_<Wrap, std::shared_ptr<Hector::Element::ElementBase>, boost::noncopyable, py::bases<ElementBaseWrap> >( name, py::no_init )
      .def( "clone", py::pure_virtual( &Hector::Element::ElementBase::clone ), py::return_value_policy<py::return_by_value>() )
      .def( "matrix", py::pure_virtual( &Hector::Element::ElementBase::matrix ) )
    ;
  }

  template<class T,class init=py::init<std::string,float,float,float>,class P=Hector::Element::ElementBase>
  void convertElement( const char* name )
  {
//    py::class_<T, std::shared_ptr<Hector::Element::ElementBase>, py::bases<P> >( name, init() )
    py::class_<T, py::bases<P> >( name, init() )
      .def( "clone", &T::clone, py::return_value_policy<py::return_by_value>() )
//      .def( "clone", &T::clone, py::return_value_policy<py::manage_new_object>() )
      .def( "matrix", &T::matrix )
    ;
  }
}

#endif

