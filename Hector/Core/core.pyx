cimport hector

cdef class HepVector:
    cdef hector.HepVector* thisptr # c++ instance to be wrapped
    def __cinit__(self):
        self.thisptr = new hector.HepVector()
    def __cinit__(self, num_coord, default):
        self.thisptr = new hector.HepVector(num_coord, default)
    def __dealloc__(self):
        del self.thisptr

