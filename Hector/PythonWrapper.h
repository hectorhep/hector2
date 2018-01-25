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

  template<class T, class init = py::init<std::string,float,float,float> >
  void convertElement( const char* name )
  {
    py::class_<T, py::bases<Hector::Element::ElementBase> >( name, init() )
      .def( "clone", &T::clone, py::return_value_policy<py::return_by_value>() )
      .def( "matrix", &T::matrix )
    ;
  }
}

#endif

