cdef extern from "Propagator/StateVector.h":
    cdef cppclass StateVector:
        StateVector() except +

cdef class hectorStateVector:
    cdef StateVector *thisptr # c++ instance to be wrapped
    def __cinit__(self):
        self.thisptr = new StateVector()
    def __dealloc__(self):
        de self.thisptr

